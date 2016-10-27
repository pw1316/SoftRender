#pragma once
#include <vector>
#include <fstream>

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

namespace PW {
    inline bool isAndSetZero(float &value);

    class Rand
    {
    public:
        Rand() :start_(0) {}
        Rand(int seed) :start_(seed) {}
        ~Rand() {}
        void reset() { start_ = 0; }
        void reset(int seed) { start_ = seed; }
        float next();
    private:
        const int a_ = 16807;
        const int m_ = 0x7FFFFFFF;
        int start_;
    };

    class Vertex2F
    {
    public:
        Vertex2F() :x_(0.0f), y_(0.0f) { fixZero(); }
        Vertex2F(float posX, float posY) : x_(posX), y_(posY) { fixZero(); }
        ~Vertex2F() {}
        Vertex2F operator+(const Vertex2F &rhs) const;
        Vertex2F operator-() const;
        Vertex2F operator-(const Vertex2F &rhs) const;
        Vertex2F operator*(const float rhs) const;
        float operator*(const Vertex2F &rhs) const;
        Vertex2F & operator=(const Vertex2F &rhs);
        void normalize();
        void rotate(float angle);

        float getX();
        float getY();
        float getLength();
        void setX(float x);
        void setY(float y);

        static Vertex2F zero();
    private:
        float x_, y_;
        inline Vertex2F & fixZero();
    };

    class GenerateSyncPosition
    {
    public:
        GenerateSyncPosition();
        GenerateSyncPosition(ULONG pNum);
        ~GenerateSyncPosition();
        void generate();
        void reset();
        std::vector<Vertex2F> getServerList();
        std::vector<Vertex2F> getClientServerList();
        std::vector<Vertex2F> getClientPosition();
    private:
        //Patameters
        ULONG pointNumber_ = 100;
        const float directionRange_ = 12.0f;//(-Pi / range, Pi / range)
        const float delayMean_ = 5.0f;
        const float delaySigma_ = 4.8f;

        //Switches
        const bool accelVelocity_ = false;

        bool inited = false;
        Rand *m_pRand_ = nullptr;
        std::vector<Vertex2F> *m_pServerList_ = nullptr;
        std::vector<Vertex2F> *m_pClientServerList_ = nullptr;
        std::vector<Vertex2F> *m_pClientPositionList_ = nullptr;
    };
}
