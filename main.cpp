#include <libobsensor/ObSensor.hpp>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

static void saveFrame(const std::string &name, std::shared_ptr<ob::VideoFrame> frame){
	if(!frame){
		return;
	}

	std::ofstream out(name, std::ios::binary);
	out.write(reinterpret_cast<const char *>(frame->data()), frame->dataSize());
	out.close();

	std::cout<< name
		 << "width=" <<frame->width()
		 << "height=" <<frame->height()
		 << "format" <<frame->format()
		 << "size=" <<frame->dataSize()
		 << "index" <<frame->index()
		 << std::endl;
}
int main(){
	try{
		ob::Pipeline pipe;
		auto config=std::make_shared<ob::Config>();
		
		auto depthProfiles=pipe.getStreamProfileList(OB_SENSOR_DEPTH);
		auto depthProfile=depthProfiles->getVideoStreamProfile(0,0,OB_FORMAT_Y16,0);
		config->enableStream(depthProfile);
		try{
			auto colorProfiles=pipe.getStreamProfileList(OB_SENSOR_COLOR);
			auto colorProfile=colorProfiles->getVideoStreamProfile(0,0,OB_FORMAT_UNKNOWN,0);
			config->enableStream(colorProfile);
			}
		catch (...){
			std::cout<<"Color stream not enable, continu with depth only."<<std::endl;
			}
		pipe.start(config);
		
		for(int i=0; i<30;++i){
			auto frameset=pipe.waitForFrames(1000);
			if(!frameset){
				std::cout<<"Timeout waiting frame"<< std::endl;
				continue;
				}
			auto depth=frameset->depthFrame();
			auto color=frameset->colorFrame();

			if(i=10){
				saveFrame("depth_000.raw",depth);
				saveFrame("color_000.raw",color);
				}
			}
		pipe.stop();
		std::cout<<"Capture OK"<<std::endl;
		}
	
	catch(const std::exception &e){
		std::cerr<<"Exception: "<<e.what()<< std::endl;
		return 1;
		}
	
	catch(...){
		std::cerr<<"Unknow exception"<< std::endl;
		return 1;
		}

	return 0;
	}

