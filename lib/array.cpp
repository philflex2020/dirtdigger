/**
 * file array.cpp
 * @brief Concurrent Access Array Class.
 *
 * g++ -o array -D_MAIN array.cpp -lpthread -lrt
 */

#include <malloc.h>
#include <iostream>
#include <pthread.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <cstdlib>

using namespace std;

static int g_local_debug = 0;
static int g_peek_debug = 0;

#include "array.h"

#if 0
#define M_EXTRA2 (sizeof(int) * 2)
#define M_EXTRA (sizeof(int)) 
#define M_DUMMY 0xBADEEDAB

void *gm_malloc(int size)
{
  unsigned int *ret;
  ret = (unsigned int *)malloc(size + M_EXTRA2);
  ret[0] = M_DUMMY;  
  ret[size + M_EXTRA] = M_DUMMY;
  return (void *)&ret[1];
}


int gm_free(void *buf)
{
  unsigned int *ret = (unsigned int *)(buf - M_EXTRA);
  free(ret);
  return 0;
}
#endif

int g_array_id = 0;
Array *g_array_list = NULL;

static void array_atexit()
{
  cout << "Array Exit Handler" << endl;
  if(g_array_list) delete g_array_list;
  
}

int ArrayInit(void)
{
  const int res = atexit(array_atexit);
  cout << "Array Init Handler res ["<< res <<"]"<< endl;
  g_array_list = new Array(4096, 1024, "Array List");
  g_array_list->setId(0);  
  g_array_list->addId(0, (void *) g_array_list);
  g_array_id = 1;
  return res;
}

Array::Array()
{
    array = NULL;
    users = 0;
    max  = 0;
    inc  = 0;
    used  = 0;

    last_free = -1;
    unused  = 0;
    name =  "";
    Array(256, 128, "dummy");
}

Array::Array(int imax, int iinc, string str)
{
    int i;
    users = 0;
    max  = imax;
    inc  = iinc;
    used  = 0;

    last_free = -1;
    unused  = 0;
    name =  str;
    maxId = DEF_MAX_ID;

    if ((g_array_id > 0) && (g_array_list != NULL))
      {
	id = g_array_id++;
	g_array_list->addId(id, (void *) this);
      }
    //std::unique_ptr::<void *[]>array (new void *[max]);
    array = (void **)new void * [max];
    for (i = 0; i< max; i++)
        array[i]=0;

    pthread_mutex_init(&amutex, NULL);
}

Array::~Array()
{
    if(g_array_list) g_array_list->delId(id, (void *) this);
    delete [] array ;
    pthread_mutex_destroy(&amutex);

}

/**
 * @brief trimUnused
 * @param[none]
 * @return count of items
 */

int Array::trimUnused()
{
    int idx1 = 0;
    int idx2 = 0;
    int rc = unused;
    if(users>0) return 0;
    pthread_mutex_lock(&amutex);

    // find first instance of idx1 == NULL
    last_free = -1;

    {
      idx1 = 0;
      idx2 = 0;
      
      // move past alread used items
      
      while ((idx1 < used) && (array[idx1] != NULL))
	{
	  idx1++;
	  idx2++;
	}
      if(idx2 < used)
	{
	 while (idx2 < used) 
	   {
	  
	     if(array[idx2] == NULL)
	       {
		 idx2++;
	       }
	     else
	       {
		 array[idx1] = array[idx2];
		 array[idx2] = NULL;
		 idx1++;
		 idx2++;
	       }
	   }
	}
    }
    used = idx1;
    unused = 0;
    pthread_mutex_unlock(&amutex);
    return rc;
}

int Array::findUnusedItem()
{
    int idx;
    pthread_mutex_lock(&amutex);
    idx = _findUnusedItem();
    pthread_mutex_unlock(&amutex);
    return idx;
}

// lock already taken
int Array::_findUnusedItem()
{
    int idx = 0;
    if (last_free != -1)
        if((last_free < max) &&(array[last_free] == NULL))
        {
            idx = last_free;
            //if((last_free > 0)
            //   && (array[last_free-1] == NULL))
            //  last_free--;
            //else
            last_free = -1;
            return idx;
        }
    while (idx < max)
    {
        if (array[idx] == NULL)
        {
            return idx;
        }
        idx++;
    }
    return -1;
}


void Array::extendArray(int idx)
{
    pthread_mutex_lock(&amutex);
    _extendArray(idx);
    pthread_mutex_unlock(&amutex);
}

// must be locked
void Array::_extendArray(int idx)
{
    int imax =  max+inc+idx;
    int nmax;
    void **newArray = (void **)new void *[imax];
    //std::unique_ptr::<void *[]>newArray (new void *[imax]);
    nmax= imax;
    while (imax)
    {
        imax--;
        if(imax >= max)
        {
            newArray[imax] = NULL;
        }
        else
        {
            newArray[imax] = array[imax];
        }
    }
    max = nmax;

    delete [] array;
    array = newArray;

    return;
}
// may done internally so watch used/unused use _movItem
int Array::_movItem(void *item)
{
    int idx=-1;
    if (unused > 0) {
        idx = _findUnusedItem();
        array[idx] = item;
        //unused--;
    } else {
        if (used < max) {
            array[used] = item;
            idx = used;
            used++;
        } else {
            _extendArray();
            array[used] = item;
            idx = used;
            used++;
        }
    }

    return idx;
}

// may done internally so watch used/unused use _movItem
int Array::_addItem(void *item)
{
    int idx=-1;
    if (unused > 0) {
        idx = _findUnusedItem();
        array[idx] = item;
        unused--;
    } else {
        if (used < max) {
            array[used] = item;
            idx = used;
            used++;
        } else {
            _extendArray();
            array[used] = item;
            idx = used;
            used++;
        }
    }

    return idx;
}


int Array::addItem(void *item)
{
    int idx;
    pthread_mutex_lock(&amutex);
    idx = _addItem(item);
    pthread_mutex_unlock(&amutex);
    return idx;
}


// returns 1 if the item was added
int Array::addItemOnce(void *item)
{
    int rc = 0;
    pthread_mutex_lock(&amutex);
    if(_findItem(item) < 0)
    {
        rc = 1;
        _addItem(item);
    }
    pthread_mutex_unlock(&amutex);
    return rc;
}


// also sets last used
// this is also covered with setmax stuff
// no more last_free
// in a push / pop queue unused ponts to the first to pop.
int Array::pushItem(void *item)
{
    int idx=-1;
    pthread_mutex_lock(&amutex);

    if (used >= max) {
        _extendArray();
    }
    array[used] = item;
    idx = used;
    used++;
    //    if(last_free == -1)
    //  last_free = idx;

    pthread_mutex_unlock(&amutex);
    return idx;
}

// done checked this
// pushitem
void *Array::popItem(void)
{
    void *item = NULL;
    pthread_mutex_lock(&amutex);
    if(unused < used)
      {
	item = array[unused];
	array[unused] = NULL;
	if(item)
	  {
	    if(last_free == -1) last_free = unused;
	    unused++;
	  }
      }
    else
      {
	last_free = -1;
	used = 0;
	unused = 0;
      }
    pthread_mutex_unlock(&amutex);
    return item;
}

// used in copy
void *Array::peekAt(int idx)
{
    int i;
    void *item = NULL;
    pthread_mutex_lock(&amutex);

    // start at unused and then count
    i = unused + idx;
    if(i < used)
      {
	item = array[i];
      }
    pthread_mutex_unlock(&amutex);
    return item;
}

// can only be used after a push
void *Array::peekItem(void)
{

    void *item = NULL;
    pthread_mutex_lock(&amutex);
    if(unused < used)
      item = array[unused];
    if(g_peek_debug)
      printf("%s used %d unused %d item %p\n"
	     ,__FUNCTION__
	     , used
	     , unused
	     , (void *)item
	     );

    if(0) {
      if (last_free > -1) {
        if (last_free < used) {
	  item = array[last_free];
        } else {
	  last_free = -1;
	  used = 0;
	  unused = 0;
        }
      }
    }
    
    pthread_mutex_unlock(&amutex);
    return item;
}

int Array::clear(void)
{
    int rc = used;
    used = 0;
    unused = 0;
    return rc;
}

void *Array::getId(int idx)
{
  if ((idx >=0)&&(idx < used))
    {
      return array[idx];
    }
    return NULL;
}


int Array::delId(int idx, void *data)
{
  if(idx >= 0)
    {
      if((idx<used) && (array[idx] == data))
	{
	  pthread_mutex_lock(&amutex);
	  array[idx] = NULL;
	  if (last_free == -1) last_free = idx;
	  unused++;
	  pthread_mutex_unlock(&amutex);
	  
	}
    }
    return idx;
}


//used calc sorted it out
int Array::addId(int idx, void *data)
{
    if (idx > maxId ) return 0;
  
    int imax =  max;
    if(idx >= 0)
    {
      
      pthread_mutex_lock(&amutex);
      if(g_local_debug)
	cout << "at start .. used " << used << endl;
      
      while(idx >= max)
	{
	  _extendArray(idx);
	  if(g_local_debug)
	    cout << "Array [" << name << "] after extendArray .. "
		 << " used " << used
		 << " max " << max
		 << " idx " << idx
		 << endl;
	}
      if((imax == max) && ( idx < used))
	{
	  if(!array[idx] && (unused > 0)) unused--;
	}
      array[idx] = data;
      //unused++;
      
      while (used <= idx)
	{
	  if(array[used] == NULL)
            unused++;
	  used++;
	}
      if(g_local_debug)
	cout << "phase 1 .. used " << used
	     << " unused " << unused
	     << " imax " << imax
	     << endl;
      
      if(idx >= imax)
	{
	  while(idx > used)
	    {
	      if(array[used] == NULL)
		unused++;
	      used++;
	    }
	}
      pthread_mutex_unlock(&amutex);
    }
    return used;
}

void Array::forEach(int (*frun)(void **item, void *env), void *env)
{
    int i;
    pthread_mutex_lock(&amutex);
    for (i = 0; i < used; i++)
    {
        if(!frun(&array[i], env))
        {
            pthread_mutex_unlock(&amutex);

            return;
        }
    }
    pthread_mutex_unlock(&amutex);
}

int Array::findNextDone(void **item, int &idx, int pop)
{
    void *squatter=NULL;
    if(idx < 0) idx = 0;

    pthread_mutex_lock(&amutex);
    if((idx > 0) && (idx<=max))
    {
      if(pop)  // array[idx-1] is already NULL
	{
	  // This was a bug someone else may be here and they would be lost
	  if(item && *item)
	    {
	      unused--;  // need to put it back
	      squatter = array[idx-1];
	      array[idx-1] = *item;
	      if(squatter && (squatter != array[idx-1]))
		{
		  _movItem(squatter);
		}
	    }
	}
      else
	{
	  if(item)
	    {
	      array[idx-1] = *item;
	      if((pop == 0) &&(*item == NULL)) unused++;
	    }
	}
      if((last_free == -1) && (array[idx-1] == NULL))last_free = idx-1;
    }

    pthread_mutex_unlock(&amutex);
    return idx;
}

static int g_pop_bug = 0;
int Array::findNext(void **item, int &idx, int pop)
{
    int i;
    if(idx < 0) idx = 0;
    void *squatter = NULL;
    pthread_mutex_lock(&amutex);
    if((idx > 0) && (idx<=max))
    {
      if(pop)
	{
	  // This is a bug someone else may be here and they will be lost
	  if(item && *item)
	    {
	      unused--;
	      squatter = array[idx-1];
	      array[idx-1] = *item;
	      if(g_pop_bug==0)
		{
		  if(squatter && (squatter != array[idx-1]))
		    {
		      _movItem(squatter);
		    }
		}
	    }
	}
      else
	{

	  array[idx-1] = *item;
	}
      
      if((idx > 0) &&(*item == NULL)) {
	if(pop == 0)unused++;
	// this allows the list item to be reused during the pop
	// only done if we are sure the item is never coming back
	if((last_free == -1) && (array[idx-1] == NULL))last_free = idx-1;
      }
    }
    for (i = idx; i < used; i++)
    {
        if (array[i])
        {
            *item = array[i];
            if(pop)
            {
                array[i] = NULL;
		unused++;
            }
            idx = i+1;
            pthread_mutex_unlock(&amutex);
            return 1;
        }
    }
    pthread_mutex_unlock(&amutex);
    idx = -1;
    return 0;
}

void Array::show(int err)
{
#if 0
    cout << " Array ["<< name <<"]"
         << " max " << max
         << " used " << used
         << " unused " << unused
         << " inc " << inc
         << " array " << (void *)array
         << endl;
    int i;
    for (i = 0; i < used; i++)
    {
        cout << " item[" << i << "]"
             << " value [" << array[i] << "]"
             << endl;
    }
#endif
    if(g_local_debug)
        syslog(err, "Array [%s] max %d used %d unused %d (inc %d) \n"
               , name.c_str()
               , max
               , used
               , unused
               , inc
              );
    int i;
    for (i = 0; i < used; i++)
    {
        if(g_local_debug)
            syslog(err, "item[%d] value [%p]\n"
                   , i
                   , array[i]
                  );
    }

}

int Array::_findItem(void *item)
{
    int idx = 0;
    //    pthread_mutex_lock(&amutex);
    for (; idx < used; idx++)
      {
        if(array[idx] == item )
        {
	  //pthread_mutex_unlock(&amutex);
            return idx;
        }
      }
    //pthread_mutex_unlock(&amutex);
    return -1;
}


int Array::findItem(void *item)
{
    int idx = 0;
    pthread_mutex_lock(&amutex);
    idx = _findItem(item);
    pthread_mutex_unlock(&amutex);
    return idx;
}

int Array::_delItem(void *item)
{
    int idx = 0;
    int dc=0;
    pthread_mutex_lock(&amutex);

    for (; idx < used; idx++)
        if(array[idx] == item )
        {
            array[idx]=NULL;
            if (last_free == -1) last_free = idx;
            unused++;
            dc++;
        }
    pthread_mutex_unlock(&amutex);

    return dc;
}



void *Array::findMatch(void *item, int (*match)(void * i1, void *i2, void*env), void *env)
{
    int idx = 0;
    pthread_mutex_lock(&amutex);

    for (; idx < used; idx++)
        if(match(array[idx], item , env))
        {
            pthread_mutex_unlock(&amutex);
            return array[idx];
        }
    pthread_mutex_unlock(&amutex);

    return NULL;
}


int Array::delMatch(void *item, int (*match)(void * i1, void *i2, void *env), void *env)
{
    int idx = 0;
    int dc = 0;
    pthread_mutex_lock(&amutex);

    for (; idx < used; idx++)
        if(match(array[idx], item , env))
        {
            array[idx]=NULL;
            if (last_free == -1) last_free = idx;
            dc++;
            unused++;
        }
    pthread_mutex_unlock(&amutex);
    return dc;
}

// Buffer stuff
/*
struct Buffer{

  Buffer();
  Buffer(int max, int inc, string name);
  ~Buffer();
  int setData(char *data, int len);
  char *getData(void);
  char *setUsed(int used);
  int getLen(void);

private:
  string name;
  unsigned char *data;
  int used;
  int max;
  int inc;
  pthread_mutex_t amutex;
};
*/

Buffer::Buffer()
{

    data = NULL;
    used = 0;
    max = 0;
    len = 0;
    inc =  16;
    cycles = 0;
    pthread_mutex_init(&amutex, NULL);

}
Buffer::~Buffer()
{
    if(data)free(data);
    data = NULL;
    pthread_mutex_destroy(&amutex);

}

int Buffer::setData(unsigned char *indata, int inlen)
{
    pthread_mutex_lock(&amutex);

    if(indata)
    {
        if(inlen >= max)
        {
            if(data)free(data);
            data = (unsigned char *)malloc(inlen+inc);
            max = inlen+inc;
        }
        memcpy((void *)data, (void *)indata, inlen);
        data[inlen] = 0;
        len = inlen;
        used = 0;
    }
    pthread_mutex_unlock(&amutex);

    return 0;

}

#if 0
int Buffer::setData2(unsigned char *indata, int inlen)
{
    unsigned char *ndata;
    pthread_mutex_lock(&amutex);

    if(indata)
    {
        if(inlen >= max)
        {
            if(data)
	      {
		ndata = (unsigned char *)malloc(inlen+inc);
		max = inlen+inc;
		memcpy((void *)ndata, (void *)&data[used], len-used);

		free(data);
		len -= used;
		used = 0;
		data = ndata 
	      }
        }
        memcpy((void *)&data[used], (void *)indata, inlen);
        data[inlen] = 0;
        len = inlen;
        used = 0;
    }
    else
      {
	if(inlen >= max)
	  {
            if(data)free(data);
            data = (unsigned char *)malloc(inlen+inc);
            max = inlen+inc;
	    data[0]=0;
	    used = 0;
	    len =0;
	  }

      }
    pthread_mutex_unlock(&amutex);

    return 0;

}
#endif

int Buffer::setData(string indata)
{
  return setData((unsigned char *)indata.c_str(), indata.length());
}

// new feature add the buffer length 
int Buffer::addLen(int inlen)
{
    pthread_mutex_lock(&amutex);
    if(inlen < max)
      {
	len += inlen;
	data[len] = 0;
      }
    else
      {
	inlen = 0;
      }
    pthread_mutex_unlock(&amutex);
    return inlen;
}

// new feature set the buffer length 
// deletes old buffer data
int Buffer::setLen(int inlen)
{
    pthread_mutex_lock(&amutex);
    if(inlen >= max)
      {
	if(data)free(data);
	data = (unsigned char *)malloc(inlen+inc);
	max = inlen+inc;
      }
    //memcpy((void *)data, (void *)indata, inlen);
    data[0] = 0;
    len = 0;
    used = 0;
    pthread_mutex_unlock(&amutex);
    return 0;
}


int Buffer::showData(string prompt)
{
  unsigned char *bdata;
  int alen;
  int rlen;
  int amax;
  
  bdata = getData();
  rlen = getRealLen();
  alen = getLen();
  amax = getMax();
  cout <<" Buffer "
      << prompt
      << "[";
  if(bdata)
    cout << bdata;
  cout << "] Reallen ["
       << rlen
       << "] alen ["
       << alen
        << "]"
       << "] amax ["
       << amax
        << "]"
       << endl;
  return rlen;
}

// new feature append data to the buffer
int Buffer::addData(unsigned char *indata, int inlen)
{
    pthread_mutex_lock(&amutex);
    unsigned char *ndata = NULL;
    if(len + inlen >= max)
      {
	//if(data)free(data);
	ndata = (unsigned char *)malloc(len +inlen+inc);
	if(data)
	  {
	    if(len)memcpy((void *)ndata, (void *)data, len);
	    free(data);
	    
	  }
	data =  ndata;

	max = used + inlen+inc;
      }
    if(indata)
      {
	memcpy((void *)&data[len], (void *)indata, inlen);
	data[len + inlen] = 0;
	len = len+inlen;
      }
    else
      {
	data[len] = 0;
      }

    pthread_mutex_unlock(&amutex);
    return 0;
}

int Buffer::addData(string indata)
{
  return addData((unsigned char *)indata.c_str(), indata.length());
}

unsigned char *Buffer::getData()
{
    unsigned char *ret = NULL;
    if(data && (used < len))
        ret = &data[used];
    return ret;
}

unsigned char *Buffer::getPtr()
{
    unsigned char *ret = NULL;
    if(data && (len < max ))
        ret = &data[len];
    return ret;
}


int Buffer::getLen()
{
    int ret = 0;
    if(used <= len)
        ret = len - used;
    return ret;
}

int Buffer::getRealLen()
{
    return len;
}

int Buffer::getUsed()
{
    return used;
}
#if 0
int Buffer::setUsed(int ulen)
{
  if((used + ulen) < len)
    used += ulen;
  else
    used = len;
  
    return used;
}
#endif



int Buffer::setUsed(unsigned char *ustr)
{
  int ulen = strlen((char *)ustr);
  return setUsed(ulen);
}

int Buffer::setUsed(string ustr)
{
  int ulen = ustr.length();
  return setUsed(ulen);
}

int Buffer::getMax()
{
    return max;
}

int Buffer::getSpace()
{
    return max-len;
}

// gets space and extends buffer if needed
// tobe tested
int Buffer::getSpace(int rlen)
{
  if(rlen >= (max-len))
    {
      addData(NULL, rlen);
    }
  return max-len;
  
}


int Buffer::setUsed(int inlen)
{
    pthread_mutex_lock(&amutex);

    if(data)
    {
        if((used + inlen) < len)
        {
            used += inlen;
        }
        else
        {
            if(g_peek_debug)
                cout << " Buffer setUsed used " << used
                     << " inlen " << inlen
                     << " len "  << len
                     << endl;
            used = 0;
            len = 0;
        }
    }
    pthread_mutex_unlock(&amutex);

    return used;
}

int Buffer::testLen(int inlen, int inmax)
{
    int ok = 0;
    pthread_mutex_lock(&amutex);
    if(data)
    {
        if((used == 0) && (inlen < max) && inmax?(inmax>max):1)
        {
            ok = 1;
        }
    }
    pthread_mutex_unlock(&amutex);

    return ok;
}

int BuffQue::bufShow(char *note)
{
  int idx = 0;
  Buffer *buf = NULL;
  if(note)
    cout<<" BuffQue Data for   <<" << note << ">>" << endl;
    
  cout<<" BufQ  " << name << endl;
  cout<<" Qarray  used " << qarray->getUsed()<< " unused " << qarray->getunUsed()<<endl;

  //qarray->show();
  
  while (qarray->findNext((void **)&buf, idx, 0)) {
    cout <<" BUFF " << (void *) buf << "  ";
    buf->showData("qArray ..");
  }
  
  cout<<" Barray  " << endl;
  idx = 0;  buf = NULL;
  while (barray->findNext((void **)&buf, idx, 0)) {
    cout <<" BUFF " << (void *) buf << "  ";
    buf->showData("bArray ..");
  }
   
  //cout<<" Barray  " << endl;
  //barray->show();
  return 0;
}
//send_fd
//setMax
string BuffQue::bufPull(int len)
{
  //int rc=-1;
  Buffer *buf=NULL;
  string ustr("Not found");
  unsigned char *usp;
  int plen;
  buf = static_cast<Buffer *>(qarray->peekItem());
  if(buf)
    {
      usp = buf->getData();
      plen = buf->getLen();
      if(usp)
	{
	  if(len == 0) len=plen;
	  if(plen < len)
	    len = plen;
	  ustr=string((const char*)usp, (size_t)len);
	  buf_done(qarray, barray, len);

	}
    }
  return ustr;
}

int BuffQue::bufFindStr(string str)
{
  int rc=-1;
  Buffer *buf=NULL;
  string ustr;
  unsigned char *usp;
  //int plen;
  buf = static_cast<Buffer *>(qarray->peekItem());
  if(buf)
    {
      usp = buf->getData();
      //plen = buf->getLen();
      if(usp)
	{
	  ustr=string((const char*)usp);
	  rc = (int) ustr.find(str);
	}
    }
  return rc;
}

int buf_delete(Array *barray)
{
    Buffer *buf=NULL;
    int idx = 0;
    while(barray->findNext((void **)&buf, idx, 1)) {
        delete buf;
        buf = NULL;
    }
    return 0;
}

Buffer *buf_find(Array *barray, int len)
{
    Buffer *buf=NULL;
    int idx = 0;
    int fidx = -1;
    while(barray->findNext((void **)&buf, idx, 1)) {
      if((len == 0 ) || (buf->testLen(len, len*2)))
        {
	  if(g_local_debug)
	    cout << " buf_find reuse of buf " << buf <<endl;
	  if (fidx == -1) fidx = idx-1;
	  //barray->findNextDone(NULL, idx);
	  //return(buf);
	  
        }
      
    }
    if(fidx == -1)
      {
    // second run with no max
	buf=NULL;
	idx = 0;
	while(barray->findNext((void **)&buf, idx, 1)) {
	  if(buf->testLen(len, 0))
	    {
	      if(g_local_debug)cout << " buf_find reuse of buf " << buf <<endl;
	      if (fidx == -1) fidx = idx-1;
	      
	      //barray->findNextDone(NULL, idx);
	      //return(buf);
	      
	    }
	  
	}
      }

    if (fidx == -1)
      {
	buf = NULL;
      }
    else
      {
	buf = (Buffer *)barray->getId(fidx);
	barray->delId(fidx, buf);
      }

    return buf;
}

// Added the bq structure to allow limited size queues
int buf_queue(BuffQue *bq, Array *barray, Array *qarray, unsigned char *sp, int len)
{
    Buffer *buf;

    buf = buf_find(barray, len);
    if(!buf)
    {
        buf = new Buffer;
    }
    //    printf(" %s got new or old buf \n", __FUNCTION__);
    buf->cycles++;
    buf->setData(sp, len);
    qarray->pushItem((void *)buf);

    // very neat a limited depth bq
    // use setMax for this
    if(bq && bq->maxq)
      {
	bq->count++;
	if(bq->count> bq->maxq)
	  {
	    buf = static_cast<Buffer *>(qarray->popItem());
	    barray->addItem((void *)buf);
	  }
	if(bq->count > bq->maxcount)
	  {
	    qarray->trimUnused();
	    bq->count = bq->maxq;
	  }
	
      }
    return len;
}

// this tries to find a buffer on the barray (used) with at least a given length
// if it fails it creates a new one
// and puts it on the qarray (active)
//
void *buf_get(Array *barray, Array *qarray,  int len)
{
    Buffer *buf;

    buf = buf_find(barray, len);
    if(!buf)
    {
        buf = new Buffer;
    }
    buf->cycles++;
    buf->setLen(len);
    qarray->pushItem((void *)buf);

    return (void *)buf;
}

// used for read data
// gives the current data pointer and length
// of the item on the qarray (active)
// usp = &data[used] plen = len-used
unsigned char *buf_get(Array *qarray, int *plen)
{
    unsigned char*usp = NULL;
    Buffer *buf;
    int len = 0;
    buf = static_cast<Buffer *>(qarray->peekItem());
    if(buf)
    {
        usp = buf->getData();
        len = buf->getLen();
    }
    if(plen) *plen = len;
    return usp;
}

// tells us that we are done with len data in the top
// qarray buffer
// will pop and return to barray if we are done
// see buf_send_fd
// we should probably lock the whole buffer for this
int buf_done(Array *qarray, Array *barray, int len)
{
    int rc;
    Buffer *buf;
    buf = static_cast<Buffer *>(qarray->peekItem());
    rc =  buf->setUsed(len);
    if(rc == 0)
    {
        qarray->popItem();
        barray->addItem((void *)buf);
    }
    return rc;
}



// send buffer to fd
// use errno to differentiate between partial writes and 0 length writes
unsigned int g_numb=0;
unsigned int g_numc=0;
int buf_send_fd(Array *qarray, Array *barray, int fd)
{
  return buf_send_fd_err(qarray, barray, fd, NULL);
}

int buf_send_fd_err(Array *qarray, Array *barray, int fd, int *err)
{

    int len;
    int rc;
    if(err) *err = 0;
    do {
        unsigned char *usp;
        g_numb++;
        usp = buf_get(qarray, &len);
        if(usp)
        {
            //if(len >5) len = 5;
            rc =  write(fd, usp, len);
	    if(rc == -1)
	      {
		if(err)*err = errno;
		if(errno == EWOULDBLOCK) {
		  rc = 0;
		}
	      }
            if(rc > 0)
            {
                g_numc += rc;
		// this will mark how much has been sent
		// if we sent it all we will move to  the next buffer if we have one
                buf_done(qarray, barray, rc);
            }
            // capture partial send
	    // if rc was < len we only sent a partial,
	    // buf_get (above) will get the new pointer to the remainder and the
	    // new length to send on the next poll cycle.
            if((rc > 0) && (rc < len))
                rc = 0;
        }
        else
        {
            rc = 0;
        }

    } while (rc > 0);
    return rc;
}

// given an array find the sum of the buffer lengths
int buf_len(Array *qarray)
{
  int len = 0;
  Buffer *buf=NULL;
  int idx = 0;
  while(qarray->findNext((void **)&buf, idx, 0)) {
    len += buf->getLen();
    if(g_local_debug)
      cout << " buf_len  " << buf->getLen() << " total len " << len << endl;
  }
  return len;
}

// either create a new buffer with the sum of all data in qarray
// of find on on the barray
//
int buf_merge(Array *qarray, Array *barray, int dep)
{

    int len;
    int mydep=0;
    unsigned char *usp;
    Buffer *buf=NULL;
    Buffer *bufp=NULL;

    bufp =  new Buffer;
    //(Buffer *)qarray->popItem();
    mydep = 0;
    //if(dep == 0)
    {
        do
	  {
	    if(!buf)buf =  (Buffer *)qarray->popItem();
	    if(buf)
	      {
		usp = buf->getData();
		len = buf->getLen();
		bufp->addData(usp, len);
		if(g_local_debug)
		  printf(" %s buf %p len %d usp [%s]\n"
			 , __FUNCTION__, (void *)buf, len, usp);
		barray->addItem((void *)buf);
		if((dep == 0) || (mydep++ < dep))
		  {
		    buf =  (Buffer *)qarray->popItem();
		  }
		else
		  {
		    buf=NULL;
		  }
	      }
	  } while(buf);
	
	usp = bufp->getData();
	len = bufp->getLen();
	//bufp->addData(usp, len);
	if(g_local_debug)
	  printf(" %s bufp_len %d usp [%s]\n", __FUNCTION__, len, usp);

	if(dep == 0)
	  {
	    qarray->pushItem((void *)bufp);
	  }
	else
	  {
	    qarray->addId(qarray->getunUsed()-1, (void *)bufp);
	  }
	if(g_local_debug)
	  printf(" %s after push_item \n", __FUNCTION__);
	
	len= bufp->getLen();
	if(g_local_debug)
	  printf(" %s after getlen  %d \n", __FUNCTION__, len);
    }
    
    return len;
}

// not really working yet
int buf_peek_fd(Array *qarray, Array *barray, int fd)
{
    int len;
    int rc;
    do {
        unsigned char *usp;
        g_numb++;
        usp = buf_get(qarray, &len);
        if(usp)
        {
            if(len >5) len = 5;
            rc =  write(1, usp, len);
            if(rc > 0)
            {
                g_numc += rc;
                buf_done(qarray, barray, rc);
            }
            // capture partial send
            if(rc < len)
                rc = 0;
        }
        else
        {
            rc = 0;
        }

    } while (rc > 0);
    return rc;
}

// send_fd
int BuffQue::setMax(int depth, int len)
{
  //unsigned char *usp;
  //int blen;
  int idx;
  Buffer *buf;
  if(maxq == 0)
    {
      maxq=depth;
      maxcount = len;
      count = 0;
    }
  else
    {
      qarray->trimUnused();
      printf(" setMax ... depth %d count %d qa->used %d qa->size %d\n"
	     , depth
	     , count
	     , qarray->getUsed()
	     , qarray->size()
	     );
      // if depth < count we need to discard
      if (depth < count)
	{
	  idx = 0;  buf = NULL;
	  while (
		 qarray->findNext((void **)&buf, idx, 1)
		 ) {
	    cout <<" BUFF " << (void *) buf << " idx " << idx ;
	    buf->showData(">>>qArray ..");
	    count--;
	    barray->addItem((void *)buf);
	    buf = NULL;
	    if(depth == count)
	      {
		//qarray->findNextDone(NULL, idx);
		break;
	      }
	    
	  }
   	}
      cout << "after qarray reduction used "<< qarray->getUsed()
	   << " unused "<< qarray->getunUsed()
	   << " size "<< qarray->size()
	   << endl;
      idx = 0; buf = NULL;
      while (qarray->findNext((void **)&buf, idx, 0)) {
	cout <<" BUFF " << (void *) buf << " idx " << idx ;
	buf->showData(">>>qArray ..");
      }

      qarray->trimUnused();
      cout << "after qarray trim unused " << endl;
      idx = 0; buf = NULL;
      while (qarray->findNext((void **)&buf, idx, 0)) {
	cout <<" BUFF " << (void *) buf << " idx " << idx ;
	buf->showData(">>>qArray ..");
      }

 
      count = qarray->size();
      maxq = depth;
      maxcount = len;	  
    }
  return maxq;
}

//send_fd

BuffQue *BuffQue::copy(BuffQue *bq)
{
  
  
  Buffer *buf = NULL;

  int len;
  unsigned char *usp = NULL;
  int idx = 0;
  bq->setMax(maxq, maxcount);
  do {
    buf = (Buffer *)peekAt(idx++);
    if(buf)
      {
	usp = buf->getData();
	len = buf->getLen();
	bq->bufSet(usp, len);
      }
  } while (buf);
  return bq;
}

//send_fd
BuffQue *BuffQue::copy(char *iname)
{
  
  BuffQue *bq = new BuffQue(iname);
  return copy(bq);
}

#if 0
  Buffer *buf = NULL;

  int len;
  unsigned char *usp = NULL;
  int idx = 0;
  bq->setMax(maxq, maxcount);
  do {
    buf = (Buffer *)peekAt(idx++);
    if(buf)
      {
	usp = buf->getData();
	len = buf->getLen();
	bq->bufSet(usp, len);
      }
  } while (buf);
  return bq;
}
#endif

#ifdef _MAIN
#define showTest \
  do { \
    cout << endl << " ******* Running test <"	\
	 << __FUNCTION__\
	 << ">  ***********"\
	 << endl;\
} while (0);

int test_buffer()
{
    int rc;
    string str;

    showTest;
    
    g_local_debug = 1;
    //cout<<" Test_buffer " << endl;

    Array *barray = new Array(4, 16, "buffers");
    Array *qarray = new Array(4, 16, "Queue");
    str = "This is long string 1\n";
    buf_queue(NULL, barray, qarray, (unsigned char *)str.c_str(), str.length());
    str = "This is string 2\n";
    buf_queue(NULL, barray, qarray, (unsigned char *)str.c_str(), str.length());
    str = "This is string 3\n";
    buf_queue(NULL, barray, qarray, (unsigned char *)str.c_str(), str.length());
    cout<<" Qarray  " << endl;
    qarray->show();
    cout<<" Barray  " << endl;
    barray->show();
    cout<<" buf_send_fd <<<<<" << endl;
    rc  = buf_send_fd(qarray, barray, 1);
    cout<<" >>>> buf_send_fd" << endl;

    cout<<" Num cycles " << g_numb<< endl;
    cout<<" Num chars " << g_numc<< endl;
    cout<<" Qarray  " << endl;
    qarray->show();
    cout<<" Barray  " << endl;
    barray->show();
    str = "This is string 1\n";
    buf_queue(NULL,barray, qarray, (unsigned char *)str.c_str(), str.length()) ;
    str = "This is much much much longer string 2\n";
    buf_queue(NULL,barray, qarray, (unsigned char *)str.c_str(), str.length()) ;
    str = "This is string 3\n";
    buf_queue(NULL,barray, qarray, (unsigned char *)str.c_str(), str.length()) ;
    cout<<" Qarray  " << endl;
    qarray->show();
    cout<<" Barray  " << endl;
    barray->show();
    if(0)cout<<" >> rc  " << rc << endl;
    cout<<" buf_send_fd <<<<<" << endl;

    rc  = buf_send_fd(qarray, barray, 1);
    cout<<">>>>> buf_send_fd" << endl;

    cout<<" >> rc  " << rc << endl;
    cout<<" Num cycles " << g_numb<< endl;
    cout<<" Num chars " << g_numc<< endl;
    cout<<" Qarray  " << endl;
    qarray->show();
    cout<<" Barray  " << endl;
    barray->show();

    buf_delete(barray);
    buf_delete(qarray);
    delete barray;
    delete qarray;
    

    return 0;
}

// in this test we want to recover the latest buffer to add data to it
// test 1 extend the buffer
// test 2 use the buffer in a read operation
// sp = buf->getPtr();
// len = buf->getSpace();
// rc=read(fd, sp, len)
// if (rc > 0 ) buf->setLen(rc)
// then abstract the whole operation to the bq level
//buf_send_fd(qarray, barray, 1);
//}
// test3 check the limited size buffer
// bq->setMax(int depth, int max);


int test_buffQ2()
{
  //int rc;
    string str;
    Buffer *buf;
    unsigned char *sp;
    int len;
    int nlen;
    
    showTest;

    buf = new Buffer;
    buf->setData("dummy stuff hi Buffer <<");
    g_local_debug = 1;
    buf->showData(" After Set .. ");
    buf->setUsed("dummy stuff ");
    buf->showData(" After Set used .. ");
    buf->addData("this is me >>");
    buf->showData(" After Add .. ");
    delete buf;

    buf = new Buffer;
    // create space in the new buffer
    if(0)
      {
	buf->setLen(512);
	buf->showData(" After Setlen .. ");
      }
    // simulate read to put data in the buffer (may need a lock)
    len   = buf->getSpace(512);  // may need to request a min size
    buf->showData(" After getSpace(512) .. ");
    //len   = buf->getSpace();  // may need to request a min size
    sp   = buf->getPtr();
    printf(" len %d , sp %p\n", len, sp);
    // simulate a read 
    nlen = snprintf((char *)sp, len,"%s", "Add some stuff <<");
    // set new data len into buffer
    buf->addLen(nlen< len? nlen: len);
    
    buf->showData(" After addlen 1 .. ");
    // try it all again
    //len   = buf->getSpace(512);
    len   = buf->getSpace();
    sp   = buf->getPtr();
    nlen = snprintf((char *)sp, len, "%s", "Add some more stuff>>");
    buf->addLen(nlen< len? nlen: len);
    buf->showData(" After addlen 2 .. ");

    BuffQue *bq = new BuffQue(" Test BuffQ");
    bq->setMax( 5, 10);
    bq->bufSet("this is string 1");
    bq->bufSet("this is string 2");
    bq->bufSet("this is string 3");
    bq->bufSet("this is string 4");
    bq->bufSet("this is string 5");
    bq->bufShow( (char *)"inital queue ");
    bq->bufSet("this is string 6");
    bq->bufSet("this is string 7");
    bq->bufShow( (char *)"after string 7 ");
    bq->bufSet("this is longer string 8");
    bq->bufSet("this is longer string 9");
    bq->bufSet("this is longer string 10");
    bq->bufSet("this is string 11");
    bq->bufShow( (char *)"after string 11 ");
    BuffQue *bq2 = bq->copy((char *)" Copy of Test BuffQ");

    bq2->bufShow( (char *)"bq2 after bq copy ");
    bq->setMax( 3, 6);
    bq->bufShow( (char *)"bq after bq setmax 3 6");
    bq->bufSet("this is string 12");
    bq->bufShow( (char *)"bq string 12");

    bq->setMax( 5, 9);
    bq->bufShow( (char *)"after bq setmax 5 9");
    bq->bufSet("this is string 13");
    bq->bufSet("this is string 14");
    bq->bufSet("this is string 15");
    bq->bufShow( (char *)"bq2 after string 15");

    delete bq;
    delete bq2;

    //int BuffQue::setMax(int depth, int len)
#if 0

    Buffer *buf;
    //buf = static_cast<Buffer *>(bq->bufGet(1024));
    //if(!buf)
    //{

    //}
    // we want a min space 
    //bq->setLen(1024);
    len  = bq->getSpace(1024);
    sp   = bq->getPtr();
    nlen = snprintf((char *)sp, len, "%s", "Add some more stuff>>");
    bq->addLen(nlen< len? nlen: len);
    
    bq->bufShow();
    buf = static_cast<Buffer *>(bq->bufGet(0));
    buf->showData(" From BQ .. ");

    delete bq;
#endif
    
    delete buf;
    return 0;

}



int test_buff_merge()
{
    int rc;
    string str;
    //Buffer *buf;
    showTest;

    BuffQue *bq = new BuffQue(" Test Buffer Merge");

    rc = bq->bufReady();
    cout<<" ready  1 >> " << rc << endl;


    str = "This is long string 1  ";
    bq->bufSet(str) ;
    str = "This is string 2\n";
    bq->bufSet(str) ;
    str = "This is string 3\n";
    bq->bufSet(str) ;

    bq->bufShow();
    rc = bq->bufReady();
    cout<<" bufReady  2  >> " << rc << endl;
    str = "\n";
    rc = bq->bufFindStr(str);
    cout<<" After buffFindStr  1  rc (-1 ok)>> " << rc << endl;
    
    bq->bufMerge();
    //buf_merge
    printf(" %s after bufMerge \n", __FUNCTION__);

    //cout<<" ready  3  now show after merge rc (-1 notok) >> " << rc << endl;
    rc = bq->bufFindStr(str);
    cout<<" After bufFind 2  rc (-1 not now ok)>> " << rc << endl;
    str =  bq->bufPull(rc + str.length());
    cout<<" After bufPull 2  str[ " << str<< "].." << endl;

    bq->bufShow();
    
    g_local_debug = 0;
    rc = bq->bufSendFd(1);

    
    cout<<" Num cycles " << g_numb<< endl;
    cout<<" Num chars " << g_numc<< endl;
    if(0)cout<<" >> rc  " << rc << endl;

    rc = bq->bufReady();
    cout<<" ready  3 >> " << rc << endl;

    g_local_debug = 1;
    bq->bufShow();



    delete bq;
    return 0;

}

int test_buffQ()
{
    int rc;
    string str;
    showTest;

    g_local_debug = 1;
    //cout<<" Test_buffer Queue " << endl;
    BuffQue *bq = new BuffQue(" Test Buffer");

    rc = bq->bufReady();
    cout<<" ready  1 >> " << rc << endl;


    str = "This is long string 1\n";
    bq->bufSet(str) ;
    str = "This is string 2\n";
    bq->bufSet(str) ;
    str = "This is string 3\n";
    bq->bufSet(str) ;

    bq->bufShow();
    rc = bq->bufReady();
    cout<<" ready  2 >> " << rc << endl;

    rc = bq->bufSendFd(1);

    cout<<" Num cycles " << g_numb<< endl;
    cout<<" Num chars " << g_numc<< endl;
    if(0)cout<<" >> rc  " << rc << endl;

    rc = bq->bufReady();
    cout<<" ready  3 >> " << rc << endl;

    bq->bufShow();


    str = "This is string 1\n";
    bq->bufSet(str) ;
    str = "This is much much much longer string 2\n";
    bq->bufSet(str) ;
    str = "This is string 3\n";
    bq->bufSet(str) ;

    bq->bufShow();

    rc  = bq->bufSendFd(1);

    cout<<" >> rc  " << rc << endl;
    cout<<" Num cycles " << g_numb<< endl;
    cout<<" Num chars " << g_numc<< endl;
    bq->bufShow();

    delete bq;

    return 0;
}

int test_dict()
{
    showTest;

    Array *darray = new Array(4, 16, "dict");
    cout<<" dict at start " << endl;
    darray->show();
    darray->addId(1, (void *)1);
    cout<<" addID(1,1) " << endl;
    darray->show();
    
    darray->addId(5, (void *)5);
    cout<<" addID(5,5) " << endl;
    darray->show();

    darray->addId(5, (void *)0x55);
    cout<<" addID(5,55) " << endl;
    darray->show();

    darray->addId(3, (void *)3);
    cout<<" addID(3,3) " << endl;
    darray->show();

    darray->addId(7, (void *)7);
    cout<<" addID(7,7) " << endl;
    darray->show();


    delete darray;
    return 0;
}

// test function for match
int amatch(void *i1, void *i2, void *env)
{
    if (i1 == i2)
        return 1;
    return 0;
}

// test function for forEach
// rc  = 0 to terminate transition
int ashow(void **i1, void *env)
{
    int rc = 1;
    if (i1 == env)
        rc = 0;

    cout << __FUNCTION__
         << " env " <<  env
         << " item " << *i1
         << " ret " << rc
         << endl;
    return rc;
}


#define DAEMON_NAME "test_array"
int test_array ()
{
    showTest;

    Array *array = new Array(4, 16, "test");
    void * foo;
    int rc;

    array->addItem((void *)1);
    array->show();
    array->addId(32, (void *)2);
    array->addId(22, (void *)3);
    array->show();
    array->addItem((void *)4);
    array->show();

    array->addItem((void *)2);
    array->show();
    array->addItem((void *)3);
    array->show();
    delete array;
    if(1)return 1;
    array->addItem((void *)4);
    array->addItem((void *)4);
    array->addItem((void *)4);
    array->show();
    array->addItem((void *)5);
    array->show();
    cout << " Hello from Array size "<< array->size() << " empty " << array->empty() << endl;
    foo = array->findMatch((void *)2, amatch, NULL);
    cout << " Found foo " << foo << endl;
    array->show();
    array->forEach(ashow, (void *)3);

    array->_delItem((void *)3);
    array->show();

    array->addItem((void *)31);
    array->show();
    rc = array->delMatch((void *)4, amatch, NULL);
    cout << " After delMatch rc = " << rc << endl;
    array->show();
    int i = 0;
    void *aitem=NULL;
    while (array->findNext(&aitem, i))
    {
        cout << " find next i = " << i
             << " item = " << aitem
             << endl;
        // remove this one
        if(i == 2) aitem = NULL;
        array->show();
    }

    rc = array->trimUnused();
    cout << " After trimUnused rc = " << rc << endl;
    array->show();

    delete array;
    return 0;
}

int test_push ()
{
    showTest;

    void *item = NULL;
    Array *array = new Array(4, 16, "push");
    array->pushItem((void *)1);
    array->pushItem((void *)2);
    array->pushItem((void *)3);
    array->pushItem((void *)4);
    array->show();

    do {

        item = array->peekItem();
        cout << " After Peek item  " << item << endl;
        array->show();

        item = array->popItem();
        cout << " After Pop item  " << item << endl;
        array->show();

    } while(item);

    delete array;
    return 0;
}


int test_buff_merge2()
{
    int rc;
    string str;
    //Buffer *buf;
    showTest;

    BuffQue *bq = new BuffQue(" Test Buffer Merge");

    rc = bq->bufReady();
    cout<<" ready  1 >> " << rc << endl;


    str = "This is long string 1  ";
    bq->bufSet(str) ;
    str = "This is string 2\n";
    bq->bufSet(str) ;
    str = "This is string 3\n";
    bq->bufSet(str) ;

    bq->bufShow();
    rc = bq->bufReady();
    cout<<" bufReady  2  >> " << rc << endl;
    str = "\n";
    rc = bq->bufFindStr(str);
    cout<<" After buffFindStr  1  rc (-1 ok)>> " << rc << endl;
    
    bq->bufMerge(1);
    //buf_merge
    printf(" %s after bufMerge  1 \n", __FUNCTION__);
    bq->bufShow();

    //cout<<" ready  3  now show after merge rc (-1 notok) >> " << rc << endl;
    rc = bq->bufFindStr(str);
    cout<<" After bufFind 2  rc (-1 not now ok)>> " << rc << endl;
    str =  bq->bufPull(rc + str.length());
    cout<<" After bufPull 2  str[ " << str<< "].." << endl;

    bq->bufShow();
    
    g_local_debug = 0;
    rc = bq->bufSendFd(1);

    
    cout<<" Num cycles " << g_numb<< endl;
    cout<<" Num chars " << g_numc<< endl;
    if(0)cout<<" >> rc  " << rc << endl;

    rc = bq->bufReady();
    cout<<" ready  3 >> " << rc << endl;

    g_local_debug = 1;
    bq->bufShow();



    delete bq;
    return 0;

}

#define NUM_THREADS 16

Array *g_array;
int g_iret [NUM_THREADS];
sem_t g_sem;

void *atest_fcn(void *data)
{
    int items = 0;
    int *i = (int *)data;
    cout << " Thread " << *i
         << " started "
         << endl;
    sem_wait(&g_sem);
    g_iret[*i] = 0;
    int idx = 0;
    void *aitem;
    while (g_array->findNext(&aitem, idx))
    {
        items++;
        cout << " Thread " << *i
             << " find next idx = " << idx
             << " item = " << aitem
             //<< " items "<< items
             << endl;
        // remove this one
        //if(idx == 2)
        aitem = NULL;
        //g_array->show();

    }
    g_iret[*i] = items;

    pthread_exit(NULL);
}


int test_threads(void)
{
    showTest;

    long i;
    int idx [NUM_THREADS];
    int data [NUM_THREADS];
    pthread_t thread[NUM_THREADS];
    g_array = new Array(4, 16, "threads");
    sem_init(&g_sem, 0, 0);

    for (i = 0; i<NUM_THREADS; i++)
    {

        data[i] = i;
        g_array->addItem((void *)(i+1));
        g_array->addItem((void *)((i+1)<<8));

        idx[i] = pthread_create(&thread[i], NULL, atest_fcn, (void *)&data[i]);
    }

    for (i = 0; i<NUM_THREADS; i++)
    {
        sem_post(&g_sem);
    }

    long  items =0;
    for (i = 0; i<NUM_THREADS; i++)
    {

        pthread_join(thread[i], NULL);
        cout << " Thread " << i
             << " idx " << idx[i]
             << " finished processed "
             << g_iret[i] << " items"
             << endl;
        items += g_iret[i];

    }
    printf(" All threads done total items  %ld\n"
           , items
          );

    g_array->show();
    delete g_array;
    return 0;
}

int test_addId()
{
    showTest;

    Array *darray = new Array(4, 16, "dict");
    cout<<" dict at start " << endl;
    darray->show();
    darray->addId(1, (void *)1);
    cout<<" addID(1,1) " << endl;
    darray->show();
    
    darray->addId(5, (void *)5);
    cout<<" addID(5,5) " << endl;
    darray->show();

    darray->addId(5, (void *)0x55);
    cout<<" addID(5,55) " << endl;
    darray->show();

    darray->addId(3, (void *)3);
    cout<<" addID(3,3) " << endl;
    darray->show();

    darray->addId(7, (void *)7);
    cout<<" addID(7,7) " << endl;
    darray->show();

    darray->addId(124, (void *)8);
    cout<<" addID(124,8) " << endl;
    darray->show();


    delete darray;
    return 0;
}

int test_pop()
{
    long int i;
    int idx;
    void *item;
    showTest;
    Array *darray = new Array(4, 16, "pop");
    cout<<" pop at start " << endl;
    darray->show();
    for (i = 0 ; i < 16; i++)
      darray->addItem((void *)(i+0x10));
    cout<<" pop after add " << endl;
    darray->show();
    for (i = 0 ; i < 4; i++)
      darray->_delItem((void *)(i+0x10));
    cout<<" pop after del " << endl;
    darray->show();


    idx = 0;
    item = NULL;
    while(darray->findNext((void**)&item, idx, 1)) {
      //if (idx == 2 ) item= NULL;
      if (idx == 8 ) {
	darray->show();
	cout<<" pop at idx 7 " << endl;
	darray->addId(7,(void *)(0x1234));
	darray->show();
      }
    }
    cout<<" pop at end " << endl;
    darray->show();

    delete darray;
    return 0;
}

int test_next_done()
{
    long int i;
    int idx;
    void *item;
    showTest;
    Array *darray = new Array(4, 16, "pop");
    cout<<" done at start " << endl;
    darray->show();
    for (i = 0 ; i < 16; i++)
      darray->addItem((void *)(i+0x10));
    cout<<" done after add " << endl;
    darray->show();
    for (i = 3 ; i < 8; i++)
      darray->_delItem((void *)(i+0x10));
    cout<<" done after del  size ["<< darray->size() <<"]"<< endl;
    darray->show();


    idx = 0;
    item = NULL;
    while(darray->findNext((void**)&item, idx, 1)) {
      //if (idx == 2 ) item= NULL;
      cout<<" findNext idx [" <<idx-1
	  << "] size [" << darray->size() <<"]"<< endl; 
      if (idx == 9 ) {
	item=NULL;
	darray->findNextDone((void**)&item, idx, 1);
	cout<<" done at idx 8 "
	    << " size [" << darray->size() <<"]"<< endl; 

	darray->show();
	break;

	darray->addId(2,(void *)(0x1234));
	darray->show();
      }
    }
    cout<<" done at end "
	<< " size [" << darray->size() <<"]"<< endl; 
    
    darray->show();

    delete darray;
    return 0;
}





int main(int argc, char *argv[])
{
  //g_array_id = -1;
    int res = ArrayInit();
    cout << "Array Init Handler res ["<< res <<"]"<< endl;

    int opt_daemonize = 0;
    int log_options = opt_daemonize ? 0 : LOG_PERROR;
    openlog(DAEMON_NAME, log_options, LOG_LOCAL0);


    test_array();
    if(argc > 1)
        test_threads();
    test_push();
    test_buffer();
    test_buffQ();
    test_dict();
    test_buffQ2();
    test_buff_merge();
    test_addId();
    test_buff_merge2();
    cout<<" g_array_list " << endl;
    if(g_array_list)g_array_list->show();
    g_pop_bug = 1;
    cout<< endl;
    cout<<" with pop bug" << endl;
    test_pop();
    g_pop_bug = 0;
    cout<< endl;
    cout<<" without pop bug" << endl;
    test_pop();
    test_next_done();

    return 0;

}
#endif
