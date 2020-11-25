#ifndef SIG_INTER_H
#define SIG_INTER_H

struct sig_inter {
    unsigned int sig_num : 6;
    unsigned int sig_statis : 26;
};

#endif //SIG_INTER_H