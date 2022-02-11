#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

# include "myfirmata.h"


// opening message
//uint8_t buf[3];
//buf[0] = START_SYSEX;
//buf[1] = REPORT_FIRMWARE; // read firmata name & version
//buf[2] = END_SYSEX;
//port.Write(buf, 3);
//void MyFrame::

//void DoMessage(int fd);

void DoMessage(int fd);

typedef struct {
	uint8_t mode;
	uint8_t analog_channel;
	uint64_t supported_modes;
	uint32_t value;
} pin_t;


pin_t pin_info[128];

uint8_t parse_buf[4096];
int parse_command_len; 
int parse_count; 
int tx_count; 
int rx_count; 
char *firmata_name = NULL;

void Parse(int fd, const uint8_t *buf, int len)
{
  const uint8_t *p, *end;
  //int parse_command_len; 
  p = buf;
  end = p + len;
  for (p = buf; p < end; p++) {
    uint8_t msn = *p & 0xF0;
    if (msn == 0xE0 || msn == 0x90 || *p == 0xF9) {
      parse_command_len = 3;
      parse_count = 0;
    } else if (msn == 0xC0 || msn == 0xD0) {
      parse_command_len = 2;
      parse_count = 0;
    } else if (*p == START_SYSEX) {
      parse_count = 0;
      parse_command_len = sizeof(parse_buf);
    } else if (*p == END_SYSEX) {
      parse_command_len = parse_count + 1;
    } else if (*p & 0x80) {
      parse_command_len = 1;
      parse_count = 0;
    }
    if (parse_count < (int)sizeof(parse_buf)) {
      parse_buf[parse_count++] = *p;
    }
    if (parse_count == parse_command_len) {
      DoMessage(fd);
      parse_count = parse_command_len = 0;
    }
  }
}

//void MyFram
void add_pin(int pin)
{
  char str [4096];
  int slen = 0;
  //wxString *str = new wxString();
  //str->Printf("Pin %d", pin);
  slen += snprintf(&str[slen], sizeof(str) - slen,"Pin %d", pin);
  //wxStaticText *pin_name = new wxStaticText(scroll, -1, *str);
  //	add_item_to_grid(pin, 0, pin_name);

  //	wxArrayString list;
  //	if (pin_info[pin].supported_modes & (1<<MODE_INPUT)) list.Add("Input");
  //	if (pin_info[pin].supported_modes & (1<<MODE_OUTPUT)) list.Add("Output");
  //	if (pin_info[pin].supported_modes & (1<<MODE_ANALOG)) list.Add("Analog");
  //	if (pin_info[pin].supported_modes & (1<<MODE_PWM)) list.Add("PWM");
  //	if (pin_info[pin].supported_modes & (1<<MODE_SERVO)) list.Add("Servo");
  //	wxPoint pos = wxPoint(0, 0);
  //	wxSize size = wxSize(-1, -1);
  //	wxChoice *modes = new wxChoice(scroll, 8000+pin, pos, size, list);
  //	if (pin_info[pin].mode == MODE_INPUT) modes->SetStringSelection("Input");//
  //	if (pin_info[pin].mode == MODE_OUTPUT) modes->SetStringSelection("Output");
  //	if (pin_info[pin].mode == MODE_ANALOG) modes->SetStringSelection("Analog");
  //	if (pin_info[pin].mode == MODE_PWM) modes->SetStringSelection("PWM");
  //	if (pin_info[pin].mode == MODE_SERVO) modes->SetStringSelection("Servo");
  //	printf("create choice, mode = %d (%s)\n", pin_info[pin].mode,
  //		(const char *)modes->GetStringSelection());
  //	add_item_to_grid(pin, 1, modes);
  //	modes->Validate();
  //	wxCommandEvent cmd = wxCommandEvent(wxEVT_COMMAND_CHOICE_SELECTED, 8000+pin);
	//modes->Command(cmd);
	//	OnModeChange(cmd);
}

int send_analog(int chan, int val, uint8_t *buf)
{
  uint8_t cmd = 0xE0;
  buf[0] = cmd & 0xF0 | chan & 0x0F;
  buf[1] = uint8_t(val & 0xFF);
  buf[2] = uint8_t(val>>7 & 0xFF);
  return 3;
}

int send_input(int num, int val, uint8_t *buf)
{
  uint8_t cmd = 0x90;
  buf[0] = cmd & 0xF0 | num & 0x0F;
  buf[1] = uint8_t(val & 0xFF);
  buf[2] = uint8_t(val>>7 & 0xFF);
  return 3;
}


void DoMessage(int fd)
{
  uint8_t cmd = (parse_buf[0] & 0xF0);
  
  //printf("message, %d bytes, %02X\n", parse_count, parse_buf[0]);
  
  if (cmd == 0xE0 && parse_count == 3) {
    int analog_ch = (parse_buf[0] & 0x0F);
    int analog_val = parse_buf[1] | (parse_buf[2] << 7);
    for (int pin=0; pin<128; pin++) {
      if (pin_info[pin].analog_channel == analog_ch) {
	pin_info[pin].value = analog_val;
	printf("pin %d is A%d = %d\n", pin, analog_ch, analog_val);
	//wxStaticText *text = (wxStaticText *)
	//FindWindowById(5000 + pin, scroll);
	//if (text) {
	//wxString val;
	printf("A%d: %d", analog_ch, analog_val);
	//text->SetLabel(val);
      }
      return;
    }
  }

  else if (cmd == 0x90 && parse_count == 3) {
    int port_num = (parse_buf[0] & 0x0F);
    int port_val = parse_buf[1] | (parse_buf[2] << 7);
    int pin = port_num * 8;
    printf("port_num = %d, port_val = %d\n", port_num, port_val);
    for (int mask=1; mask & 0xFF; mask <<= 1, pin++) {
      if (pin_info[pin].mode == MODE_INPUT) {
	uint32_t val = (port_val & mask) ? 1 : 0;
	if (pin_info[pin].value != val) {
	  printf("pin %d is %d\n", pin, val);
	  //wxStaticText *text = (wxStaticText *)
	  //FindWindowById(5000 + pin, scroll);
	  //if (text) text->SetLabel(val ? "High" : "Low");
	  pin_info[pin].value = val;
	}
      }
    }
    return;
  }  else if (parse_buf[0] == START_SYSEX && parse_buf[parse_count-1] == END_SYSEX) {
    // Sysex message
    if (parse_buf[1] == REPORT_FIRMWARE) {
      char name[140];
      int len=0;
      for (int i=4; i < parse_count-2; i+=2) {
	name[len++] = (parse_buf[i] & 0x7F)
	  | ((parse_buf[i+1] & 0x7F) << 7);
      }
      name[len++] = '-';
      name[len++] = parse_buf[2] + '0';
      name[len++] = '.';
      name[len++] = parse_buf[3] + '0';
      name[len++] = 0;
      firmata_name = strdup(name);;
      // query the board's capabilities only after hearing the
      // REPORT_FIRMWARE message.  For boards that reset when
      // the port open (eg, Arduino with reset=DTR), they are
      // not ready to communicate for some time, so the only
      // way to reliably query their capabilities is to wait
      // until the REPORT_FIRMWARE message is heard.
      uint8_t buf[80];
      len=0;
      buf[len++] = START_SYSEX;
      buf[len++] = ANALOG_MAPPING_QUERY; // read analog to pin # info
      buf[len++] = END_SYSEX;
      buf[len++] = START_SYSEX;
      buf[len++] = CAPABILITY_QUERY; // read capabilities
      buf[len++] = END_SYSEX;
      for (int i=0; i<16; i++) {
	buf[len++] = 0xC0 | i;  // report analog
	buf[len++] = 1;
	buf[len++] = 0xD0 | i;  // report digital
	buf[len++] = 1;
      }
      write(fd, buf, len);
      tx_count += len;
    } else if (parse_buf[1] == CAPABILITY_RESPONSE) {
      int pin, i, n;
      for (pin=0; pin < 128; pin++) {
	pin_info[pin].supported_modes = 0;
      }
      for (i=2, n=0, pin=0; i<parse_count; i++) {
	if (parse_buf[i] == 127) {
	  pin++;
	  n = 0;
	  continue;
	}
	if (n == 0) {
	  // first byte is supported mode
	  pin_info[pin].supported_modes |= (1<<parse_buf[i]);
	}
	n = n ^ 1;
      }
      // send a state query for for every pin with any modes
      for (pin=0; pin < 128; pin++) {
	uint8_t buf[512];
	int len=0;
	if (pin_info[pin].supported_modes) {
	  buf[len++] = START_SYSEX;
	  buf[len++] = PIN_STATE_QUERY;
	  buf[len++] = pin;
	  buf[len++] = END_SYSEX;
	}
	//port.W
	write(fd, buf, len);
	tx_count += len;
      }
    } else if (parse_buf[1] == ANALOG_MAPPING_RESPONSE) {
      int pin=0;
      for (int i=2; i<parse_count-1; i++) {
	pin_info[pin].analog_channel = parse_buf[i];
	pin++;
      }
      return;
    } else if (parse_buf[1] == PIN_STATE_RESPONSE && parse_count >= 6) {
      int pin = parse_buf[2];
      pin_info[pin].mode = parse_buf[3];
      pin_info[pin].value = parse_buf[4];
      if (parse_count > 6) pin_info[pin].value |= (parse_buf[5] << 7);
      if (parse_count > 7) pin_info[pin].value |= (parse_buf[6] << 14);
      add_pin(pin);
    }
    return;
  }
}
