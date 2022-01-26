/*
htop - MemoryNodeMeter.c
Added to support numa memory node information
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "MemoryNodeMeter.h"

#include <math.h>
#include <stddef.h>

#include "CRT.h"
#include "Object.h"
#include "Platform.h"
#include "RichString.h"

#ifdef MEMNODE_ON
static const int MemoryNodeMeter_attributes[] = {
   MEMORY_USED,
   MEMORY_SHARED,
};

static void MemoryNodeMeter_init(Meter* this) {
   unsigned int nodeNum = this->param;
   if (this->pl->memNodes > 1) {
      char caption[10];
      xSnprintf(caption, sizeof(caption), "M%2u", nodeNum);
      Meter_setCaption(this, caption);
   }
}

static void MemoryNodeMeter_getUiName(const Meter* this, char* buffer, 
      size_t length) {
      xSnprintf(buffer, length, "%s %u", Meter_uiName(this), this->param);
}

static void MemoryNodeMeter_updateValues(Meter* this) {
   char* buffer = this->txtBuffer;
   size_t size = sizeof(this->txtBuffer);
   unsigned int node = this->param;
   int written;

   /* shared and available memory are not supported on all platforms
    * plus, buffers, cache are not supported on all platforms
    */
   this->values[1] = NAN;
   this->values[2] = NAN;
   this->values[3] = NAN;
   this->values[4] = NAN;
   Platform_setMemoryNodeValues(this, node);

   /* only support used mem for now */
   this->curItems = 2;

   written = Meter_humanUnit(buffer, this->values[0], size);
   METER_BUFFER_CHECK(buffer, size, written);

   METER_BUFFER_APPEND_CHR(buffer, size, '/');

   Meter_humanUnit(buffer, this->total, size);
}

static void MemoryNodeMeter_display(const Object* cast, RichString* out) {
   char buffer[50];
   const Meter* this = (const Meter*)cast;

   RichString_writeAscii(out, CRT_colors[METER_TEXT], ":");
   Meter_humanUnit(buffer, this->total, sizeof(buffer));
   RichString_appendAscii(out, CRT_colors[METER_VALUE], buffer);

   Meter_humanUnit(buffer, this->values[0], sizeof(buffer));
   RichString_appendAscii(out, CRT_colors[METER_TEXT], " used:");
   RichString_appendAscii(out, CRT_colors[MEMORY_USED], buffer);

   /* buffers is not supported on all platforms */
   if (!isnan(this->values[1])) {
       Meter_humanUnit(buffer, this->values[1], sizeof(buffer));
       RichString_appendAscii(out, CRT_colors[METER_TEXT], " buffers:");
       RichString_appendAscii(out, CRT_colors[MEMORY_BUFFERS_TEXT], buffer);
   }

   /* shared memory is not supported on all platforms */
   if (!isnan(this->values[2])) {
      Meter_humanUnit(buffer, this->values[2], sizeof(buffer));
      RichString_appendAscii(out, CRT_colors[METER_TEXT], " shared:");
      RichString_appendAscii(out, CRT_colors[MEMORY_SHARED], buffer);
   }

   /* cache is not supported on all platforms */
   if (!isnan(this->values[3])) {
       Meter_humanUnit(buffer, this->values[3], sizeof(buffer));
       RichString_appendAscii(out, CRT_colors[METER_TEXT], " cache:");
       RichString_appendAscii(out, CRT_colors[MEMORY_CACHE], buffer);
   }

   /* available memory is not supported on all platforms */
   if (!isnan(this->values[4])) {
      Meter_humanUnit(buffer, this->values[4], sizeof(buffer));
      RichString_appendAscii(out, CRT_colors[METER_TEXT], " available:");
      RichString_appendAscii(out, CRT_colors[METER_VALUE], buffer);
   }
}

const MeterClass MemoryNodeMeter_class = {
   .super = {
      .extends = Class(Meter),
      .delete = Meter_delete,
      .display = MemoryNodeMeter_display,
   },
   .updateValues = MemoryNodeMeter_updateValues,
   .getUiName = MemoryNodeMeter_getUiName,
   .defaultMode = BAR_METERMODE,
   .maxItems = 10,
   .total = 100.0,
   .attributes = MemoryNodeMeter_attributes,
   .name = "MemNode",
   .uiName = "MemNode",
   .caption = "Nod",
   .init = MemoryNodeMeter_init
};
#endif
