#include <stdio.h>
#include <stdlib.h>

#ifndef _NOTIFICATIONS_H
#define _NOTIFICATIONS_H

 //  STRUCTS
typedef struct __notification {
    uint32_t id;            // Notification id
    char author[20];        // User name
    uint32_t timestamp;     // Notification timestamp
    char* message;          // Notification message
    uint16_t length;        // Message length
    uint16_t pendingCount;  // Amount of users that haven't yet received the notification
} notification;