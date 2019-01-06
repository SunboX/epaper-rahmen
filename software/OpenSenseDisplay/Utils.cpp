#include "Utils.h"

String IPAddress2String(const IPAddress &ipaddress)
{
  char myIpString[24];
  sprintf(myIpString, "%d.%d.%d.%d", ipaddress[0], ipaddress[1], ipaddress[2], ipaddress[3]);
  return String(myIpString);
}

void debug_out(const String &text, const int level, const bool linebreak)
{
  //  if (level <= debug) {
  //    if (linebreak) {
  //      Serial.println(text);
  //    } else {
  //      Serial.print(text);
  //    }
  //  }
}
