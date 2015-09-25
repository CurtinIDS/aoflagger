#ifndef AOFLAGGER_UNITTEST_H
#define AOFLAGGER_UNITTEST_H

#include <string>
#include <vector>
#include <iostream>

#include "../../util/stopwatch.h"

#include "testitem.h"

class UnitTest : public TestItem {
	public:
		UnitTest(const std::string &name) : _name(name), _successes(0), _failures(0)
		{
		}
		
		virtual ~UnitTest()
		{
			for(std::vector<RunnableTest*>::iterator i=_tests.begin();i!=_tests.end();++i)
			{
				delete *i;
			}
		}
		
		template<typename Functor>
		void AddTest(Functor testFunctor, const std::string &name)
		{
			_tests.push_back(new SpecificTest<Functor>(testFunctor, name));
		}
		
		void Run()
		{
			for(std::vector<RunnableTest*>::iterator i=_tests.begin();i!=_tests.end();++i)
			{
				std::cout << "* Running subtest '" << (*i)->_name << "'... " << std::flush;
				try {
					Stopwatch watch(true);
					(*i)->Run();
					if(watch.Seconds()>0.0)
						std::cout << "SUCCESS (" << watch.ToShortString() << ")\n";
					else
						std::cout << "SUCCESS\n";
					++_successes;
				} catch(std::exception &exception)
				{
					std::cout << "FAIL\n\nDetails of failure:\n" << exception.what() << "\n\n";
					++_failures;
				}
			}
		}
		
		const std::string &Name() const { return _name; }
		unsigned Successes() const { return _successes; }
		unsigned Failures() const { return _failures; }
	private:
		struct RunnableTest {
			public:
				RunnableTest(const std::string &name) : _name(name) { }
				virtual ~RunnableTest() { }
				virtual void Run() = 0;
				std::string _name;
			private:
				RunnableTest(const RunnableTest &) { }
				void operator=(const RunnableTest &) { }
		};
		
		template<typename Functor>
		struct SpecificTest : public RunnableTest {
			SpecificTest(Functor functor, const std::string &name) : RunnableTest(name), _functor(functor)
			{
			}
			virtual void Run()
			{
				_functor();
			}
			Functor _functor;
		};

		std::vector<RunnableTest*> _tests;
		std::string _name;
		unsigned _successes, _failures;
};

#endif
