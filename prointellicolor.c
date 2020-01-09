/* prointellicolor - Util for setting taillight color for the Microsoft Pro IntelliMouse.
 * Copyright 2020 namazso <admin@namazso.eu>
 * 
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For more information, please refer to <http://unlicense.org/>
 */

#include <stdio.h>
#include <stdlib.h>

#include "hidapi.h"

hid_device* open_device(unsigned short vendor_id, unsigned short product_id, unsigned short usage)
{
  hid_device* dev = NULL;
  struct hid_device_info *devs, *cur_dev;
  devs = hid_enumerate(vendor_id, product_id);
  cur_dev = devs;
  while (cur_dev)
  {
    if (cur_dev->usage == usage)
    {
      dev = hid_open_path(cur_dev->path);
      break;
    }
    cur_dev = cur_dev->next;
  }
  hid_free_enumeration(devs);

  return dev;
}

int set_color(hid_device* dev, unsigned char r, unsigned char g, unsigned char b)
{
  unsigned char buf[73];

  memset(buf, 0, sizeof(buf));

  buf[0] = 0x24;
  buf[1] = 0xB2;
  buf[2] = 0x03;
  buf[3] = (unsigned char)r;
  buf[4] = (unsigned char)g;
  buf[5] = (unsigned char)b;
  
  return hid_send_feature_report(dev, buf, sizeof(buf));
}

int main(int argv, char** argc)
{
  int r, g, b;
  hid_device* dev;
  int res, ret = 0;

  if(argv < 4)
  {
    fprintf(stderr, "usage: prointellicolor <r> <g> <b>\n");
    ret = 1;
    goto free_none;
  }

  r = atol(argc[1]);
  g = atol(argc[2]);
  b = atol(argc[3]);

  if(r > 255 || g > 255 || b > 255)
  {
    fprintf(stderr, "error: value out of bounds\n");
    ret = 2;
    goto free_none;
  }

  if (hid_init())
  {
    fprintf(stderr, "error: cannot initialize hidapi\n");
    ret = 3;
    goto free_none;
  }

  dev = open_device(0x045E, 0x082A, 0x0212);
  if(!dev)
  {
    fprintf(stderr, "error: cannot open device\n");
    ret = 4;
    goto free_hid;
  }

  res = set_color(dev, r, g, b);
  if (res < 0)
  {
    fprintf(stderr, "error: cannot write device: %ws\n", hid_error(dev));
    ret = 5;
    goto free_dev;
  }

free_dev:
  hid_close(dev);

free_hid:
  hid_exit();

free_none:
  return ret;
}
