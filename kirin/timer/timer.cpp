#include "kirin/timer/timer.h"

BEGIN_KIRIN_NS(timer);

uint64_t estimate_cpu_frequency() {
    uint64_t start = rdtsc();
    sleep(1);
    return (rdtsc() - start) / 1000000;
}

uint64_t get_cpu_frequency() {
    static uint64_t freq = 0;
    char buf[1024];

    if (freq != 0) return freq;

    FILE* fp = fopen("/proc/cpuinfo", "r");
    const std::string splitter = ":";
    if (fp != NULL) {
        while (fgets(buf, sizeof(buf), fp) != NULL) {
            std::string buf_str = buf;
            std::vector<std::string> res = common::string_util::split(buf_str, splitter);
            if (res.size() == 2 && strncasecmp(res[0].data(), "cpu MHz", 7) == 0) {
                double freq_mhz = strtod(res[1].data(), NULL);
                freq = static_cast<uint64_t>(freq_mhz * 1000000);
                printf("cpu frequency is %ld\n", freq);
                break;
            }
        }

        fclose(fp);
    }

    if (freq == 0) return estimate_cpu_frequency();

    return freq;
}

END_KIRIN_NS(timer);
