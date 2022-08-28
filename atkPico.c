#include "atk.pio.h"

#include "atkPico.h"

#include "hardware/clocks.h"
#include "hardware/pio.h"

static PIO atk_pio;         // pio0 or pio1
static uint atk_pio_irq; 
static uint atk_sm;         // pio state machine index
static uint atk_connected;


static uint sending_to_keyboard = 0;
static Ps2LockingKeysUnion lockedKeys;

void __attribute__((weak)) handle_ps2_keyboard_event(unsigned char ps2Key, unsigned char released, unsigned char ext, Ps2LockingKeysUnion *lockedKeys){}

unsigned int atk_add_parity(unsigned char byte)
{
    unsigned int returnData = byte;
    unsigned numOnes = 0;
    for(int i =0; i < 8; i++)
    {
        numOnes += byte & 1;
        byte = byte >> 1;
    }

    returnData +=  (!(numOnes & 1)) << 9;
    return returnData;
}

void handleLeds(unsigned char ps2Key, unsigned char release){


    if(sending_to_keyboard)
    {
        if(ps2Key == PS2_ACK_RESPONSE){
            if(sending_to_keyboard == PS2_COMMANDS_SET_LEDS){
                atk_send_command(atk_add_parity(lockedKeys.value));
            }
            else{
                sending_to_keyboard = 0;//??? probably not the right call.  for longer messages
            }
        }
    }
    else if(release)
    {
        if(ps2Key == PS2_CAPS_LOCK)
        {
            lockedKeys.keys.capsLock = !lockedKeys.keys.capsLock;
            atk_send_command(PS2_COMMANDS_SET_LEDS);
        }
        else if(ps2Key == PS2_SCROLL_LOCK)
        {
            lockedKeys.keys.scrollLock = !lockedKeys.keys.scrollLock;
            atk_send_command(PS2_COMMANDS_SET_LEDS);
        }
        else if(ps2Key == PS2_NUM_LOCK)
        {
            lockedKeys.keys.numLock = !lockedKeys.keys.numLock;
            atk_send_command(PS2_COMMANDS_SET_LEDS);
        }
    }        
}


void atk_keyboard_event(){
    pio_interrupt_clear(atk_pio, 0);

    static uint8_t release = 0; // Flag indicates the release of a key
    static uint8_t ext = 0;
    static uint16_t breakPauseKey = 0;

    if (pio_sm_is_rx_fifo_empty(atk_pio, atk_sm))
        return; // no new codes in the fifo
    uint8_t code = *((io_rw_8*)&atk_pio->rxf[atk_sm] + 3);
    
    //*
    //check break/pause key
    if(code == 0xE1 ||
       breakPauseKey == 245 || // check to see if it's second itteration for pause key
       breakPauseKey && code == 0x14 || //pause key
       breakPauseKey && code == 0x7e || //break key
       breakPauseKey > 0xff)            // if you sum the total of the message if it's larger 0xff it has to be a break/pause key
    {
        breakPauseKey += code;

        if(breakPauseKey == 0x4B8)
        {
            printf("found the pause key\n");
            breakPauseKey = 0;
        }
        else if(breakPauseKey == 0x3AC)
        {
            printf("found the break key\n");
            breakPauseKey = 0;
        }

        //safty if a key was bad for some reason
        if(breakPauseKey > 0x4B8){breakPauseKey = 0;} 
        return;   
    }
    //*/
    if(code == PS2_RELASE_FLAG){
        release = 1; 
    }
    else if(code == PS2_EXTENSION_FLAG)
    {
        ext = 1;
        breakPauseKey = PS2_EXTENSION_FLAG;
    }
    else{
        handleLeds(code, release);
        handle_ps2_keyboard_event(code,release,ext, &lockedKeys);

        release = 0;
        ext = 0;
        breakPauseKey = 0;
    }


}

void atk_init(uint pio, uint gpio) {
    atk_pio = pio ? pio1 : pio0;
    atk_pio_irq = pio ? PIO1_IRQ_0 : PIO0_IRQ_0;
    unsigned char dataPin = gpio;
    unsigned char clkPin = gpio + 1;

    // add internal pull up pin
    gpio_pull_up(dataPin);
    gpio_pull_up(clkPin);
    
    //setup pio
    atk_sm = pio_claim_unused_sm(atk_pio, true);
    uint offset = pio_add_program(atk_pio, &atk_program);
    pio_sm_config c = atk_program_get_default_config(offset);

    sm_config_set_set_pins(&c, gpio,2);
    sm_config_set_jmp_pin(&c, clkPin); // set the EXECCTRL_JMP_PIN

//configure out section PS2
    // Set pin directions base
    pio_sm_set_consecutive_pindirs(atk_pio, atk_sm, gpio, 2, false);
    // Set the out pins
    sm_config_set_out_pins(&c, gpio, 2);
// Shift 8 bits to the right, autopush enabled
    sm_config_set_out_shift(&c, true, true, 9); //eight data bytes plus parity

//configure in section PS2
    // Set the base input pin. pin index 0 is DAT, index 1 is CLK
    sm_config_set_in_pins(&c, gpio);

//start the pins as input at start
    uint32_t both_pins = (1u << dataPin) | (1u << (clkPin) );
    pio_sm_set_pins_with_mask(atk_pio, atk_sm, both_pins, 0);
    pio_sm_set_pindirs_with_mask(atk_pio, atk_sm, both_pins, 0);
    pio_gpio_init(atk_pio, dataPin);
    pio_gpio_init(atk_pio, clkPin);
    
    // Shift 8 bits to the right, autopush enabled
    sm_config_set_in_shift(&c, true, true, 8);

    // We don't expect clock faster than 16.7KHz and want no less
    // than 8 SM cycles per keyboard clock.
    float div = (float)clock_get_hz(clk_sys) / (8 * 16700);
    sm_config_set_clkdiv(&c, div);

    //enables IRQ for the statemachine
    pio_set_irq0_source_enabled(atk_pio, pis_interrupt0, true);
    irq_set_exclusive_handler(atk_pio_irq, atk_keyboard_event);
    irq_set_enabled(atk_pio_irq, true);

    // Ready to go
    pio_sm_init(atk_pio, atk_sm, offset, &c);
    pio_sm_set_enabled(atk_pio, atk_sm, true);
}


void atk_send_command(unsigned short data )
{
    sending_to_keyboard = data;
    pio_sm_put_blocking(atk_pio, atk_sm,data);
}

