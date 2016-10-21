#include <algorithm>
#include <cmath>
#include <fstream>
#include <windows.h>

#include "Utils.h"

#define PI 3.1415926535f

inline bool PW::isAndSetZero(float &value)
{
    if (-1e-10f < value && value < 1e-10f)
    {
        value = 0.0f;
        return true;
    }
    return false;
}

/*Class Rand*/
PW::Rand::Rand() :start_(1)
{
}

PW::Rand::Rand(int seed) : start_(seed)
{
}

PW::Rand::~Rand()
{
}

void PW::Rand::reset()
{
    start_ = 1;
}

void PW::Rand::reset(int seed)
{
    start_ = seed;
}

float PW::Rand::next()
{
    int q, r, next;
    q = m_ / a_;
    r = m_ % a_;
    next = a_ * (start_ % q) - r * (start_ / q);
    if (next < 0)
    {
        next = next + m_;
    }
    start_ = next;
    return 1.0f * next / m_;
}

/*Class Vertex2F*/
PW::Vertex2F::Vertex2F() :x_(0.0f), y_(0.0f)
{
}

PW::Vertex2F::Vertex2F(float posX, float posY) : x_(posX), y_(posY)
{
}

PW::Vertex2F::~Vertex2F()
{
}

PW::Vertex2F PW::Vertex2F::operator+(const Vertex2F & rhs) const
{
    float xx = this->x_ + rhs.x_;
    float yy = this->y_ + rhs.y_;
    return Vertex2F(xx, yy);
}

PW::Vertex2F PW::Vertex2F::operator-(const Vertex2F & rhs) const
{
    float xx = this->x_ - rhs.x_;
    float yy = this->y_ - rhs.y_;
    return Vertex2F(xx, yy);
}

PW::Vertex2F PW::Vertex2F::operator*(const float rhs) const
{
    float xx = this->x_ * rhs;
    float yy = this->y_ * rhs;
    return Vertex2F(xx, yy);
}

float PW::Vertex2F::operator*(const Vertex2F & rhs) const
{
    float res = this->x_ * rhs.x_ + this->y_ * rhs.y_;
    return res;
}

PW::Vertex2F & PW::Vertex2F::operator=(const Vertex2F & rhs)
{
    if (this != &rhs)
    {
        x_ = rhs.x_;
        y_ = rhs.y_;
    }
    return *this;
}

void PW::Vertex2F::normalize()
{
    float len = getLength();
    if (!isAndSetZero(len))
    {
        x_ = x_ / len;
        y_ = y_ / len;
    }
}

void PW::Vertex2F::rotate(float angle)
{
    float xx = 0.0f, yy = 0.0f;
    xx = x_ * cos(angle) - y_ * sin(angle);
    yy = x_ * sin(angle) + y_ * cos(angle);
    x_ = xx;
    y_ = yy;
    isAndSetZero(x_);
    isAndSetZero(y_);
}

float PW::Vertex2F::getX()
{
    return x_;
}

float PW::Vertex2F::getY()
{
    return y_;
}

float PW::Vertex2F::getLength()
{
    isAndSetZero(x_);
    isAndSetZero(y_);
    return sqrt(x_ * x_ + y_ * y_);
}

PW::Vertex2F PW::Vertex2F::zero()
{
    return Vertex2F();
}

PW::GenerateSyncPosition::GenerateSyncPosition()
{
    m_pServerList_ = new std::vector<Vertex2F>;
    /*Initialize random generator*/
    _SYSTEMTIME st;
    GetLocalTime(&st);
    int seed = st.wMilliseconds;
    seed = seed * 1000 + seed;
    m_pRnd = new Rand(seed);

    /*Generate path*/
    std::vector<Vertex2F> pointList;
    std::vector<Vertex2F>::iterator itPointList;
    pointList.push_back(Vertex2F(0.0f, 0.0f));
    Vertex2F pointOld;
    for (int i = 0; i < pointNumber_; i++)
    {
        float rand = m_pRnd->next();
        Vertex2F dir = pointList[pointList.size() - 1] - pointOld;
        dir.normalize();
        float len = dir.getLength();
        if (PW::isAndSetZero(len))
        {
            rand = rand * 2 * PI;
            dir = Vertex2F(cos(rand), sin(rand));
        }
        else
        {
            rand = rand / directionRange_ * 2 * PI - PI / directionRange_;
            dir.rotate(rand);
        }
        pointOld = pointList[pointList.size() - 1];
        pointList.push_back(pointOld + dir);
    }

    /*Simulate Normal Communication*/
    {
        float gameTime = 0.0f;
        Vertex2F serverPosOld;
        Vertex2F serverPos;
        Vertex2F position;
        std::vector<float> serverDelayBuf;
        std::vector<Vertex2F> serverPosBuf;
        float speed = 10.0f;
        itPointList = pointList.begin();
        while (itPointList != pointList.end())
        {
            if (serverDelayBuf.size() > 0)
            {
                for (int i = 0; i < serverDelayBuf.size(); i++)
                {
                    serverDelayBuf[i] -= 0.1;
                }
            }
            float delay = m_pRnd->next();
            delay = delay * delaySigma_ * 2.0f + delayMean_ - delaySigma_;
            delay = delay / 10.0f;
            serverPosBuf.push_back(pointList[0]);
            serverDelayBuf.push_back(delay);
            std::vector<float>::iterator itDelay = serverDelayBuf.begin();
            std::vector<Vertex2F>::iterator itPos = serverPosBuf.begin();
            float max = -10.0f;
            while (itDelay != serverDelayBuf.end())
            {
                if (isAndSetZero(*itDelay) || *itDelay < 0)
                {
                    if (*itDelay > max)
                    {
                        max = *itDelay;
                        serverPos = *itPos;
                    }
                    itDelay = serverDelayBuf.erase(itDelay);
                    itPos = serverPosBuf.erase(itPos);
                }
                else
                {
                    itDelay++;
                    itPos++;
                }
            }
            Vertex2F dir = serverPosOld - position;
            float len = dir.getLength();
            if (isAndSetZero(len))
            {
                dir.normalize();
            }
            else if (len < 1)
            {
            }
            else if (!accelVelocity_)
            {
                dir.normalize();
            }
            else
            {
                dir = dir * sqrt(log10(len));
            }
            position = position + dir;
            serverPosOld = serverPos;
            gameTime += 0.1f;

            m_pServerList_->push_back(pointList[0]);
            m_pClientNormalList_->push_back(serverPos);
            m_pClientSyncList_->push_back(position);
            itPointList++;
        }

        FILE *fp = nullptr;
        fopen_s(&fp, "pointData", "w");
        for (int i = 0; i < m_pServerList_->size(); i++)
        {
            fprintf(fp, "%f %f %f %f %f %f\n", (*m_pServerList_)[i].getX(), (*m_pServerList_)[i].getY(), (*m_pClientNormalList_)[i].getX(), (*m_pClientNormalList_)[i].getY(), (*m_pClientSyncList_)[i].getX(), (*m_pClientSyncList_)[i].getY());
        }
        fclose(fp);
        fp = nullptr;
    }
    
}

PW::GenerateSyncPosition::~GenerateSyncPosition()
{
    delete m_pRnd;
    delete m_pServerList_;
}
