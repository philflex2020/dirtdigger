/**
 * file array.h
 * Concurrent Access Array Class.
 *
 * g++ -o array -D_MAIN array.cpp -lpthread -lrt
 */



#ifndef ___ARRAY_H
#define ___ARRAY_H

#include <malloc.h>
#include <pthread.h>
#include <syslog.h>
#include <iostream>

using namespace std;
/**
 * @struct Array
 * @brief A struct represents a sparse concurrent array
 */

#define DEF_MAX_ID 65535

struct Array {

    Array();
    Array(int max, int inc, string name);
    ~Array();

    int findUnusedItem();
    int _findUnusedItem();
    void extendArray(int imax=0);
    void _extendArray(int imax=0);
    int addItem(void *item);
    int _addItem(void *item);
    int _movItem(void *item);
    int findItem(void *item);
    int _findItem(void *item);
    int pushItem(void *item);
    void *popItem(void);
    void *peekItem(void);
    void *peekAt(int idx);
    int addItemOnce(void *item);
    void *getId(int idx);
    int addId(int idx, void *data);
    int delId(int idx, void *data);

    void setId(int idx)    { id = idx;                 };
    int getMaxId(void)     { return maxId;             };
    void setMaxId(int idx) { maxId = idx;              };
    int getMax(void)       { return max;               };
    int getUsed(void)      { return used;              };
    int getunUsed(void)    { return unused;            };
    int empty(void)        { return (unused ==  used); };
    int size(void)         { return used - unused;     };
    int trimUnused(void);
    int clear(void);
    int setName(string str) { name=str; return 0;      };
    const char *getName(void) { return name.c_str();   };


    void *findMatch(void *item, int (*match)(void *i1, void *i2, void *env), void *env);

    int delMatch(void *item, int (*match)(void *i1, void *i2, void *env), void *env);
    int _delItem(void *item);
    void show(int err=LOG_DEBUG);
    void forEach(int (*frun)(void **item, void *env), void *env);
    int findNext(void **item, int &idx, int pop=1);
    int findNextDone(void **item, int &idx, int pop=1);

private:
    string name;
    int id;
    int maxId;
    void **array;
    int users;
    int max;
    int inc;
    int used;
    int unused;
    int last_free;
    pthread_mutex_t amutex;
};


/**
 * @struct Buffer
 * @brief A struct represents a data area
 */

struct Buffer {

    Buffer();
    //Buffer(int max, int inc, string name);
    ~Buffer();
    int setData(unsigned char *data, int len);
    int setData(string data);
    unsigned char *getData(void);
    unsigned char *getPtr(void);
    int setUsed(int used);
    int getLen(void);
    int testLen(int inlen, int inmax);
    int setLen(int inlen);
    int addLen(int inlen);
    int addData(unsigned char *indata, int inlen);
    int addData(string data);
    int showData(string prompt);  
    int getUsed(void);
  //int setUsed(int ulen);
    int setUsed(unsigned char *indata);
    int setUsed(string slen);
    int getMax(void);
    int getSpace(void);
    int getSpace(int rlen);
    int getRealLen(void);

public:
    unsigned int cycles;
    pthread_mutex_t amutex;

private:
    string name;
    unsigned char *data;
    int used;
    int max;
    int inc;
    int len;
  //pthread_mutex_t amutex;
};


struct BuffQue;

// delete all buffers in a queue
int buf_delete(Array *barray);

// send any data from the qarray to the fd
// park any unued buffers on the barray
int buf_send_fd_err(Array *qarray, Array *barray, int fd, int *err);
int buf_send_fd(Array *qarray, Array *barray, int fd);

// given an array find the sum of the buffer lengths
int buf_len(Array *qarray);

// send any data from the qarray to the fd
// do not consume buffers
int buf_peek_fd(Array *qarray, Array *barray, int fd);

// combines all buffers on the qarray into one
int buf_merge(Array *qarray, Array *barray, int dep);

// move a buffer from the queue array to the buffer array after
// sending all its data
int buf_done(Array *qarray, Array *barray, int len);

// get any pending data from the buffer queue also recover the data length
unsigned char *buf_get(Array *qarray, int *plen);

// find a buffer of suitable size on the barray or create a new buffer
// for the data (sp) size (len) and place it on the
// queue array
int buf_queue(BuffQue *bq, Array *barray, Array *qarray, unsigned char *sp, int len);
void *buf_get(Array *barray, Array *qarray, int len);


/**
 * @struct BuffQue
 * @brief A struct represents a pair of data buffers for write data
 *        a buffQue can have a max depth and will reset at a max count
 */

struct BuffQue {

    BuffQue(string aname)
    {
        name = aname;
        barray = new Array(4, 16, "Buffers");
        qarray = new Array(4, 16, "Queue");
	count = 0;
	maxq = 0;
	maxcount = 0;
	pthread_mutex_init(&umutex, NULL);
	pthread_mutex_init(&bmutex, NULL);
	
    };

    ~BuffQue()
    {
        pthread_mutex_destroy(&bmutex);
        pthread_mutex_destroy(&umutex);

        buf_delete(barray);
        buf_delete(qarray);

        delete barray;
        delete qarray;
    };

    int bufSet(unsigned char *sp, int len)
    {
        pthread_mutex_lock(&bmutex);
        int ret = buf_queue(this, barray, qarray, sp, len);
	pthread_mutex_unlock(&bmutex);
	return ret;
    };
    int bufSet(char *sp, int len)
    {
        pthread_mutex_lock(&bmutex);
	int ret = buf_queue(this, barray, qarray, (unsigned char *)sp, len);
	pthread_mutex_unlock(&bmutex);
	return ret;
    };
  
    void *bufGet(int len)
    {
	pthread_mutex_lock(&bmutex);
        //return buf_queue(barray, qarray, sp, len);
        void *ret =buf_get(barray, qarray, len);
	pthread_mutex_unlock(&bmutex);
	return ret;
    };

    int bufSet(string str)
    {
      //pthread_mutex_lock(&bmutex);
        int ret = bufSet((unsigned char *)str.c_str(), str.length());
      	//pthread_mutex_unlock(&bmutex);
	return ret;
    };


    int bufReady()
    {
        return qarray->size();
    };

    int bufFindStr(string str);

    string bufPull(int len);

    int bufSendFd(int fd)
    {
     	pthread_mutex_lock(&bmutex);
        int ret = buf_send_fd(qarray, barray, fd);
     	pthread_mutex_unlock(&bmutex);
	return ret;
    };

    int bufPeek(int fd)
    {
        pthread_mutex_lock(&bmutex);
        int ret = buf_peek_fd(qarray, barray, fd);
     	pthread_mutex_unlock(&bmutex);
	return ret;
    };

    int bufMerge(void)
    {
      pthread_mutex_lock(&bmutex);
      int ret =  buf_merge(qarray, barray, 0);
      pthread_mutex_unlock(&bmutex);
      return ret;
    };

    int bufMerge(int dep)
    {
      pthread_mutex_lock(&bmutex);
      int ret = buf_merge(qarray, barray, dep);
      pthread_mutex_unlock(&bmutex);
      return ret;
    };

    void *peekAt(int idx)
    {
      void *ret;
      pthread_mutex_lock(&bmutex);
      ret = qarray->peekAt(idx);
      pthread_mutex_unlock(&bmutex);
      return ret;
    };
    

    void *peekItem(void)
    {
      void *ret;
      pthread_mutex_lock(&bmutex);
      ret = qarray->peekItem();
      pthread_mutex_unlock(&bmutex);
      return ret;
    };
  
    int bufShow(char *note=NULL);

    unsigned char *sendPtr(void)
    {
        int len;
        unsigned char *usp;
        usp =  buf_get(qarray, &len);
        return usp;
    }

    int sendLen(void)
    {
        int len;
        buf_get(qarray, &len);
        return len;
    }

  
    int sendDone(int len)
    {
        pthread_mutex_lock(&bmutex);
        buf_done(qarray, barray, len);
        pthread_mutex_unlock(&bmutex);

        return 0;
    }
  int umLock(void)
  {
        pthread_mutex_lock(&umutex);
        return 0;
  }
  int umunLock(void)
  {
        pthread_mutex_unlock(&umutex);
        return 0;
  }
  int setMax(int depth, int len);

  BuffQue *copy(char *name);

  BuffQue *copy(const string iname)
  {
    return copy(iname.c_str());
  };
  
  BuffQue *copy(BuffQue *bq);
  
  int maxq;
  int maxcount;
  int count;

private:
    string name;
    Array *barray;
    Array *qarray;
    pthread_mutex_t umutex;
    pthread_mutex_t bmutex;

};


void *gm_malloc(int size);
void *gm_calloc(int size, int num);
int gm_free(void *addr);


#endif
