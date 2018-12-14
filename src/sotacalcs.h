#ifndef SOTACALCS_H
#define SOTACALCS_H


class sotaCalcs
{
public:
    sotaCalcs();
    static long long getPAExperienceForLevel(int Level);
    static long long getPAExperienceForNextLevel(int Level);
    static long long getPADecay(int Level, int hours);
    static long long getSkillExperienceForLevel(int Level);
    static long long getSkillExperienceForNextLevel(int Level);

private:
    static const long long experienceForPALevel[];
    static const long long experienceForSkillLevel[];
    static const int MAX_DECAY_TIME_HOURS;
    static const float DECAY_RATE_PER_HOUR;

};

#endif // SOTACALCS_H
