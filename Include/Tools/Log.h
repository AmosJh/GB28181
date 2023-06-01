#ifndef __LOG_H__
#define __LOG_H__

#define LOG(fmt, ...) printf("=====>[%s,%d]" fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#endif  // __LOG_H__