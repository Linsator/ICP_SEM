//#pragma once
//
//#include <string>
//#include <portaudio.h>
//#include <sndfile.h>
//#include <boost/thread.hpp>
//#include <boost/circular_buffer.hpp>
//
//struct Audio_buffer;
//
//class Audio_reader
//{
//public:
//    Audio_reader();
//    ~Audio_reader();
//
//    bool open( int idev );
//    bool start();
//    bool stop();
//    bool is_open() { return _isopen; }
//    bool running();
//    int  device();
//    static int fpb_from_sample_rate( int sampleRate );
//
//    Audio_buffer* get();
//
//private:
//    static int record_callback( const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* );
//
//    bool run();
//
//	Audio_buffer       *_buf;
//	boost::circular_buffer<Audio_buffer*> _cb;
//    PaStream           *_stream;
//    PaStreamParameters  _par;
//
//    bool _isopen;
//
//	int  _ibuf;
//
//    bool          _running;
//    boost::thread _thread;
//    boost::mutex  _mutex;
//};
