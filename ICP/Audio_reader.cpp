//#include "boost/format.hpp"
//#include "boost/date_time.hpp"
//#include <string>
//#include <iostream>
//#include <opencv2/opencv.hpp>
//
//int Audio_reader::record_callback( const void *inbuf, void *outbuf, unsigned long fpb, const PaStreamCallbackTimeInfo* timeInfo,
//    PaStreamCallbackFlags statusFlags, void *userData )
//{
//	int64 now = cv::getTickCount();
//	boost::posix_time::ptime datetime = boost::posix_time::microsec_clock::local_time();
//	static int64 last = 0;
////	std::cout << "since last " << (now-last)/cv::getTickFrequency() << std::endl;
//	last = now; //datetime;
//
//	const float  *inp  = (const float*)(inbuf);
//	Audio_reader *rdr  = (Audio_reader*)(userData);
//	Audio_buffer *buf  = rdr->_buf; //(Audio_buffer*)(userData);
//    float        *outp = (float*)(buf->data);
//
//	assert( fpb == buf->frames );
//
//    std::copy( inp, inp + buf->channels*buf->frames, outp );
//
//    buf->ready = true;
//	buf->tinf = *timeInfo;
//	buf->timestamp = 0;
//	buf->ticks = now;
//	buf->tickfreq = cv::getTickFrequency();
//	buf->datetime = datetime;
//
//	Audio_buffer *tbp = new Audio_buffer( *buf );
//	rdr->_mutex.lock();
//	rdr->_cb.push_back( tbp );
//	rdr->_mutex.unlock();
//
//    return 0;
//}
//
//Audio_reader::Audio_reader() :
//    _running(false), _stream(NULL), _isopen(false)
//{
//    PaError err;
//
//    // inicilaziace PortAudia
//    err = Pa_Initialize();
//    if ( err != paNoError ) {
//        PA_ERROR( err );
//        return;
//    }
//
//    // Nastavit velikost kruhoveho frame bufferu.
//    _cb.set_capacity(100);
//}
//
//Audio_reader::~Audio_reader()
//{
//    delete _buf;
//}
//
//bool Audio_reader::open( int idev )
//{
//    const PaDeviceInfo *pinf = Pa_GetDeviceInfo( idev );
//
//	// Pocet framu na buffer.
//	int fpb = fpb_from_sample_rate( pinf->defaultSampleRate );
//
//    // Parametry nahravani.
//    _par.device                    = idev;
//    _par.channelCount              = pinf->maxInputChannels;
//	_par.suggestedLatency          = 8. * double(fpb) / double(pinf->defaultSampleRate);
//    _par.sampleFormat              = paFloat32;
//    _par.hostApiSpecificStreamInfo = NULL;
//
//	std::cout << "suggestedLatency: " << _par.suggestedLatency << std::endl;
//
//	// Alokovat buffer pro aktualni data.
//	_buf = new Audio_buffer( _par.channelCount, fpb, 0 );
//
//    // Otevrit stream.
//    PaError err = Pa_OpenStream( &_stream, &_par, NULL, pinf->defaultSampleRate,
//	   _buf->frames, paClipOff, record_callback, this );
//
//    if ( err != paNoError ) {
//        PA_ERROR( err );
//        return false;
//    }
//
//	_isopen = true;
//    return true;
//}
//
//bool Audio_reader::start()
//{
//    // spustit samostatne vlakno read(), ktere neustale kontroluje, zda jsou k dispozici nova data
//    _thread = boost::thread( &Audio_reader::run, this );
//    return _running;
//}
//
//bool Audio_reader::stop()
//{
//    {
//        boost::mutex::scoped_lock lock( _mutex );
//        _running = false;
//    }
//
//    _thread.join();
//
//    return true;
//}
//
//// funkce Audio_reader::read() bezi v samostatnem vlaknu
//bool Audio_reader::run()
//{
//    if ( _stream ) {
//        std::cout << "Pa_StartStream( _stream ) ..." <<std::endl;
//       PaError err = Pa_StartStream( _stream );
//       if ( err != paNoError ) {
//           PA_ERROR( err );
//           std::cerr << std::endl;
//           return false;
//       }
//
//       _running = true;
//    }
//
//    while ( true ) {
//        {
//            boost::mutex::scoped_lock lock( _mutex );
//            if ( !_running ) {
//                break;
//            }
//        }
//
////		if ( _stream ) {
////            // Zkontrolovat, zda jsou dostupna data.
////			if ( _buf->ready ) {
////                boost::mutex::scoped_lock lock( _mutex );
////                Audio_buffer *buf = new Audio_buffer( *_buf );
////                _cb.push_back( buf );
////				_buf->ready = false;
//////                std::cout << "Got " << buf->frames << " frames." << std::endl;
////			}
////		}
//
//		boost::this_thread::sleep_for( boost::chrono::milliseconds(50) );
//    }
//
//    if ( _stream ) {
//        PaError err = Pa_StopStream( _stream );
//        if ( err != paNoError ) {
//            std::cerr << boost::format( "PortAudio_reader chyba: %s\n" ) % Pa_GetErrorText( err );
//        }
//    }
//
//    return true;
//}
//
//Audio_buffer* Audio_reader::get()
//{
//    boost::mutex::scoped_lock( _mutex );
//    if ( _cb.empty() )
//        return NULL;
//
//    Audio_buffer *buf = _cb.front();
//    _cb.pop_front();
//    return buf;
//}
//
//bool Audio_reader::running()
//{
//    boost::mutex::scoped_lock( _mutex );
//    return _running;
//}
//
//int Audio_reader::device()
//{
//	return _par.device;
//}
//
//int Audio_reader::fpb_from_sample_rate( int sampleRate )
//{
//	switch ( sampleRate ) {
//	case 44100:
//		return 4410;
//	case 32000:
//		return 4000;
//	case 22050:
//		return 2205;
//	case 16000:
//		return 2000;
//	default:
//		return 50; // nejvetsi spol. delitel
//	}
//}
