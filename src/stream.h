#ifndef _STREAM_H_
#define _STREAM_H_

#include <fstream>
#include <ios>


class StreamReader {

 public:
  virtual unsigned int read(void *, unsigned int size) = 0;
  virtual void close() = 0;
  virtual ~StreamReader();
};


class FileStreamReader {
 private:
  std::fstream *stream;

 public:
  FileStreamReader(const std::string &name);
  ~FileStreamReader();
  unsigned int read(void *, unsigned int size);
  void close();
  
  
};  


#endif // _STREAM_H_
