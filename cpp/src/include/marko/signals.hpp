/**************************************************\
* The MIT License (MIT)
* Copyright (c) 2014 Kevin Walchko
* see LICENSE for full details
\**************************************************/
#pragma once

#include <signal.h>
#include <unistd.h> // getpid()

/*
https://stackoverflow.com/questions/1641182/how-can-i-catch-a-ctrl-c-event
https://www.geeksforgeeks.org/inheritance-in-c/
https://stackoverflow.com/questions/12662891/how-can-i-pass-a-member-function-where-a-free-function-is-expected
http://www.yolinux.com/TUTORIALS/C++Signals.html
*/
/*
kevin@Logan build $ kill -l
 1) SIGHUP	 2) SIGINT	 3) SIGQUIT	 4) SIGILL
 5) SIGTRAP	 6) SIGABRT	 7) SIGEMT	 8) SIGFPE
 9) SIGKILL	10) SIGBUS	11) SIGSEGV	12) SIGSYS
13) SIGPIPE	14) SIGALRM	15) SIGTERM	16) SIGURG
17) SIGSTOP	18) SIGTSTP	19) SIGCONT	20) SIGCHLD
21) SIGTTIN	22) SIGTTOU	23) SIGIO	24) SIGXCPU
25) SIGXFSZ	26) SIGVTALRM	27) SIGPROF	28) SIGWINCH
29) SIGINFO	30) SIGUSR1	31) SIGUSR2
*/

/**
 * Captures a signal and calls a callback function.
 *
 * ref: https://man7.org/linux/man-pages/man2/sigaction.2.html
 */
class SignalFunc {
public:
  SignalFunc() {}
  int enable(void (*f)(int), int signum = SIGINT) {
    if (enabled) return 0;

    if (signum == SIGKILL || signum == SIGSTOP) {
      throw std::invalid_argument(
          "** Cannot capture signals SIGKILL or SIGSTOP **");
      // std::cout << "** Cannot capture signals SIGKILL or SIGSTOP **"
      //           << std::endl;
      return -1;
    }

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = f;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(signum, &sigIntHandler, NULL);

    enabled = true;
    return 0;
  }

protected:
  bool enabled{false};
};






// class SignalTerm {
//   public:
//   SignalTerm() {
//     int signum = SIGINT;
//     flag.test_and_set();

//     // if (signum == SIGKILL || signum == SIGSTOP) {
//     //   throw std::invalid_argument(
//     //       "** Cannot capture signals SIGKILL or SIGSTOP **");
//     //   // std::cout << "** Cannot capture signals SIGKILL or SIGSTOP **"
//     //   //           << std::endl;
//     //   // return -1;
//     // }

//     struct sigaction sigIntHandler;
//     sigIntHandler.sa_handler = SignalTerm::clear;
//     sigemptyset(&sigIntHandler.sa_mask);
//     sigIntHandler.sa_flags = 0;

//     sigaction(signum, &sigIntHandler, NULL);
//   }

//   static void clear(int a) { flag.clear(); std::cout<<"bye"<<std::endl;}
//   bool is_set() { return flag.test(); }

// protected:
//   static std::atomic_flag flag;
// };

// std::atomic_flag SignalTerm::flag;