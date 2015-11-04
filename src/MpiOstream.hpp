/***
 * To write outpout into a file, cout or cerr
 *
 * Inspired by http://wordaligned.org/articles/cpp-streambufs
 * use -DNULL=nullptr for C++11
 ***/

#ifndef MPI_OSTREAM_HPP
#define MPI_OSTREAM_HPP

#include <cstdlib>
#include <iostream>
#include <streambuf>
#include <fstream>
//#include <mpi.h>

namespace io {
  
  /**
   * class to write into two buffers at the same time
   */
  class mpibuf: public std::streambuf {
  public:
    // Construct a streambuf which tees output to both input streambufs.
    mpibuf(std::streambuf * sb1, std::streambuf * sb2);
    mpibuf(const mpibuf& mbuf);
    mpibuf& operator= (const mpibuf& mbuf);

  private:
    // This tee buffer has no buffer. So every character "overflows"
    // and can be put directly into the teed buffers.
    virtual int overflow(int c);
    // Sync both teed buffers.
    virtual int sync();
 
  private:
    std::streambuf * sb1;
    std::streambuf * sb2;
  };

  mpibuf::mpibuf(std::streambuf * sb1, std::streambuf * sb2) : sb1(sb1) , sb2(sb2) {}   
  mpibuf::mpibuf(const mpibuf& mbuf) : sb1(mbuf.sb1) , sb2(mbuf.sb2) {}
  
  mpibuf& mpibuf::operator= (const mpibuf& mbuf) {
    if (this != &mbuf) {
      sb1 = mbuf.sb1;
      sb2 = mbuf.sb2; 
    }
    return *this;
  }
  
  int mpibuf::overflow(int c) {
    if (c == EOF) {
      return !EOF;
    }
    else {
      int const r1 = sb1 ? sb1->sputc(c) : c;
      int const r2 = sb2 ? sb2->sputc(c) : c;
      return r1 == EOF || r2 == EOF ? EOF : c;
    }
  }
  
  int mpibuf::sync() {
    int const r1 = sb1 ? sb1->pubsync() : 0;
    int const r2 = sb2 ? sb2->pubsync() : 0;
    return r1 == 0 && r2 == 0 ? 0 : -1;
  }   
  
  
  
  
  
  class MpiStream : public std::ostream {
  public:
    // Construct an ostream which tees output to the supplied
    // ostreams.
    MpiStream(std::ostream & o1, std::ostream & o2)
      : std::ostream(&mbuf) , mbuf(o1.rdbuf(), o2.rdbuf()) { }
    MpiStream(std::ostream & o1)
      : std::ostream(&mbuf) , mbuf(o1.rdbuf(), NULL) { }
    MpiStream()
      : std::ostream(&mbuf) , mbuf(NULL, NULL) { }

    MpiStream(const MpiStream& mstream) : std::ostream(&mbuf) , mbuf(mstream.mbuf) {} 
    
    MpiStream& operator= (const MpiStream& mstream) {
      if (this != &mstream) {
	mbuf = mstream.mbuf;
      }
      return *this;
    }

  private:
    mpibuf mbuf;
  };

  static MpiStream defaultCout(std::cout);
  static MpiStream defaultCerr(std::cerr);
  MpiStream& cout = defaultCout;
  MpiStream& cerr = defaultCerr;
  
  static void setFileToIO(io::MpiStream& mstream, std::ostream& outstream, 
			  std::ofstream& filestream, std::string& newFile) {
    if ( filestream ) {
      filestream.close();
    }
    filestream.open(newFile.c_str()); 
    // todo check open
    mstream = io::MpiStream(outstream, filestream);
  }

  void setCoutFile(std::string coutFile) {
    static std::ofstream coutStream;   
    setFileToIO(io::cout, std::cout, coutStream, coutFile);
  }
  

  void setCerrFile(std::string cerrFile) {
    static std::ofstream cerrStream;
    setFileToIO(io::cerr, std::cerr, cerrStream, cerrFile);
  }


  
}

#endif // MPI_OSTREAM_HPP
