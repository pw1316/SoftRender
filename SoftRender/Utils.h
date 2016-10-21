#pragma once
#include <vector>
#include <fstream>

namespace PW {
    inline bool isAndSetZero(float &value);

    class Rand
    {
    public:
        Rand();
        Rand(int seed);
        ~Rand();
        void reset();
        void reset(int seed);
        float next();
    private:
        const int a_ = 16807;
        const int m_ = 0x7FFFFFFF;
        int start_;
    };

    class Vertex2F
    {
    public:
        Vertex2F();
        Vertex2F(float posX, float posY);
        ~Vertex2F();
        Vertex2F operator+(const Vertex2F &rhs) const;
        Vertex2F operator-(const Vertex2F &rhs) const;
        Vertex2F operator*(const float rhs) const;
        float operator*(const Vertex2F &rhs) const;
        Vertex2F & operator=(const Vertex2F &rhs);
        void normalize();
        void rotate(float angle);

        float getX();
        float getY();
        float getLength();

        static Vertex2F zero();
    private:
        float x_, y_;
    };

    class GenerateSyncPosition
    {
    public:
        GenerateSyncPosition();
        ~GenerateSyncPosition();
    private:
        //Patameters
        const size_t pointNumber_ = 80;
        const float directionRange_ = 12.0f;//(-Pi / range, Pi / range)
        const float delayMean_ = 5.0f;
        const float delaySigma_ = 1.0f;

        //Switches
        const bool noDelayOutput_ = true;
        const bool accelVelocity_ = true;

        //
        Rand *m_pRnd = nullptr;
        std::vector<Vertex2F> *m_pServerList_ = nullptr;
        std::vector<Vertex2F> *m_pClientNormalList_ = nullptr;
        std::vector<Vertex2F> *m_pClientSyncList_ = nullptr;
    };
}
