#include <pebble.h>
#include "message_helper.h"
#include "storage.h"
#include "linked-list.h"

#define WORKOUT 0
#define WORKOUT_DONE 1

#define MESSAGE_TYPE_INDEX 0
#define MESSAGE_HEADER_INDEX 1
#define MESSAGE_DATA_ROOT 2
#define MESSAGE_DATA_MAX 100


void message_helper_send_message(int type, char* message); 
bool jsReady = false; 
static AppTimer *jsReadyTimer; 
static void jsReadyTimer_callback(void* data); 

typedef struct MessageData { 
  uint8_t type; 
  char* message; 
} MessageData; 


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");
  Tuple *data_type = dict_find(iterator, MESSAGE_TYPE_INDEX); 
  
  if (data_type) {
    char * type = data_type->value->cstring; 
    APP_LOG(APP_LOG_LEVEL_INFO, "Message received with type: %s", type);

    
    char* header = dict_find(iterator, MESSAGE_HEADER_INDEX)->value->cstring; 
    
    //Put all data into a linked list and send to handlers. 
    LinkedRoot* data = linked_list_create_root(); 
    
    for (int i= MESSAGE_DATA_ROOT; i < MESSAGE_DATA_ROOT + MESSAGE_DATA_MAX; i++) { 
      Tuple* single_data = dict_find(iterator, i); 
      if (single_data){ 
        linked_list_append(data, single_data->value->cstring); 
      }
      else break; 
    }
    
    if (strcmp(type,"READY") == 0) {
      jsReady = true; 
      return;
    }; 
    if (strcmp(type,"MOVES") == 0) {
      workout_parse_message(header, data); 
    }
    if (strcmp(type,"WORKOUTS") == 0) {
      storage_store_workouts(header, data); 
    }
    
  } 
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void message_helper_init() { 
  APP_LOG(APP_LOG_LEVEL_INFO, "Message Helper Init");
  app_message_open(app_message_inbox_size_maximum(), APP_MESSAGE_OUTBOX_SIZE_MINIMUM + 500);

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
}

void message_helper_request_workout(char* workout_title) { 
  message_helper_send_message(WORKOUT, workout_title); 
}

void message_helper_finish_workout(Workout* workout) { 
  message_helper_send_message(WORKOUT_DONE, workout->name); 
}

void message_helper_send_message(int type, char* message) { 

  
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Sending message... %s %i", message, type);
  
  
  if (!jsReady) { 
    MessageData* message_data = malloc(sizeof(MessageData)); 
    message_data->message = message; 
    message_data->type = type; 

    jsReadyTimer = app_timer_register(1, jsReadyTimer_callback, message_data); 
    return; 
  }
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_cstring(iter, type, message);
  int result = app_message_outbox_send();
  switch (result){  
    case APP_MSG_OK:  
      APP_LOG(APP_LOG_LEVEL_DEBUG,"Message \"%s\" of type %i send attempt...", message, type);
      break; 
    case APP_MSG_BUSY:  //There are pending messages (in or out) that need to be processed first, try resending in a bit
      APP_LOG(APP_LOG_LEVEL_DEBUG,"BUSY");
      //TODO
      break; 
   }
}

static AppTimer *jsReadyTimer;
static void jsReadyTimer_callback(void *data){ 
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Trying to send message, phone isn't ready yet!");

    //Wait until JS is ready
    if (jsReady) { 
      MessageData* message_data = (MessageData *)(data); 
      int type = message_data->type; 
      char* message = message_data->message; 
      //TODO free message_data; 
      free(message_data); 
      message_helper_send_message(type,message);  
    }
  else { //Wait 1/10 seconds
    jsReadyTimer = app_timer_register(100 /* milliseconds */, jsReadyTimer_callback, data);  
  }
  
}

//Minified Strtok (ignore) Thanks to Steve Caldwell for trick. 
char*strtok(s,delim)
register char*s;register const char*delim;{register char*spanp;register int c,sc;char*tok;static char*last;if(s==NULL&&(s=last)==NULL)
return(NULL);cont:c=*s++;for(spanp=(char*)delim;(sc=*spanp++)!=0;){if(c==sc)
goto cont;}
if(c==0){last=NULL;return(NULL);}
tok=s-1;for(;;){c=*s++;spanp=(char*)delim;do{if((sc=*spanp++)==c){if(c==0)
s=NULL;else
s[-1]=0;last=s;return(tok);}}while(sc!=0);}}
