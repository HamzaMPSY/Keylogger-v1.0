//a class to handle the threads 
#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <chrono>

class Timer
{
	private:
	std::thread Thread;
	bool Alive = false;
	long CallNumber  = -1L;
	long Repeatcount = -1L;
   	std::chrono::milliseconds interval = std::chrono::milliseconds(0);
   	std::function<void (void) > func = nullptr;

   	void SleepAndRun(){
   		std::this_thread::sleep_for(interval);
   		if(Alive)
   			Function()();
   	}
	
	void ThreadFunc(){
		if(CallNumber == Infinite)
			while(Alive)
				SleepAndRun();
		else
			while(Repeatcount--)
				SleepAndRun();
	}

public:
	static const long Infinite = -1L;
	Timer(){}
	Timer(const std::function<void(void)> &f) : func(f){}
	Timer(const std::function<void(void)> &f,const unsigned long &i,const long repeat = Timer::Infinite) : func(f),interval(std::chrono::milliseconds(i)),CallNumber(repeat){}
	void Start(bool Async = true){
		if(IsAlive())
			return;
		Alive = true;
		Repeatcount = CallNumber;
		if(Async)
			Thread = std::thread(ThreadFunc,this);
		else 
			this->ThreadFunc();
	}

	void Stop(){
		Alive = false;
		Thread.join();
	}

	bool IsAlive() const{
		return Alive;
	}

	void SetFunction(const std::function<void(void)> &f){
		func = f;
	}

	void RepeatCount(const long r){
		if(IsAlive()){
			return;
		}
		CallNumber = r;
	}

	long GetLeftCount() const {return Repeatcount;}
	long RepeatCount()  const {return CallNumber;}
	void SetInterval(const unsigned long &i){
		if(IsAlive())
			return;
		interval = std::chrono::milliseconds(i);
	}
	unsigned long Interval() const {return interval.count();}
	const std::function<void(void)> &Function()const{
		return func;
	}
	


};

#endif						