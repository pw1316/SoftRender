#include <cmath>
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
PW::Vertex2F PW::Vertex2F::operator+(const Vertex2F & rhs) const
{
    float xx = this->x_ + rhs.x_;
    float yy = this->y_ + rhs.y_;
    return Vertex2F(xx, yy);
}

PW::Vertex2F PW::Vertex2F::operator-() const
{
    float xx = 0.0f - this->x_;
    float yy = 0.0f - this->y_;
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
    isAndSetZero(res);
    return res;
}

PW::Vertex2F & PW::Vertex2F::operator=(const Vertex2F & rhs)
{
    if (this != &rhs)
    {
        x_ = rhs.x_;
        y_ = rhs.y_;
    }
    return this->fixZero();
}

void PW::Vertex2F::normalize()
{
    float len = getLength();
    if (!isAndSetZero(len))
    {
        x_ = x_ / len;
        y_ = y_ / len;
    }
    fixZero();
}

void PW::Vertex2F::rotate(float angle)
{
    float xx = 0.0f, yy = 0.0f;
    xx = x_ * cos(angle) - y_ * sin(angle);
    yy = x_ * sin(angle) + y_ * cos(angle);
    x_ = xx;
    y_ = yy;
    fixZero();
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
    float res = sqrt(x_ * x_ + y_ * y_);
    isAndSetZero(res);
    return res;
}

void PW::Vertex2F::setX(float x)
{
    x_ = x;
    fixZero();
}

void PW::Vertex2F::setY(float y)
{
    y_ = y;
    fixZero();
}

PW::Vertex2F PW::Vertex2F::zero()
{
    return Vertex2F(0.0f, 0.0f);
}

inline PW::Vertex2F & PW::Vertex2F::fixZero()
{
    isAndSetZero(x_);
    isAndSetZero(y_);
    return *this;
}

/*Class GenPos*/
PW::GenerateSyncPosition::GenerateSyncPosition()
{
    /* Initialize random generator */
    _SYSTEMTIME st;
    GetLocalTime(&st);
    int seed = st.wMilliseconds;
    seed = seed * 1000 + seed;
    m_pRand_ = new Rand(seed);
}

PW::GenerateSyncPosition::GenerateSyncPosition(ULONG pNum)
{
    /* Initialize random generator */
    _SYSTEMTIME st;
    GetLocalTime(&st);
    int seed = st.wMilliseconds;
    seed = seed * 1000 + seed;
    m_pRand_ = new Rand(seed);
    pointNumber_ = pNum;
}

PW::GenerateSyncPosition::~GenerateSyncPosition()
{
    delete m_pRand_;
    if (m_pServerList_ != nullptr) {
        Assert(m_pClientServerList_ != nullptr);
        Assert(m_pClientPositionList_ != nullptr);
        delete m_pServerList_;
        delete m_pClientServerList_;
        delete m_pClientPositionList_;
    }
}

void PW::GenerateSyncPosition::generate()
{
    /* Generate path */
    std::vector<Vertex2F> pointList;
    std::vector<Vertex2F>::iterator itPointList;
    std::vector<float> pointDelayBuf;
    std::vector<float>::iterator itPointDelay = pointDelayBuf.begin();
    pointList.push_back(Vertex2F(0.0f, 0.0f));
    pointDelayBuf.push_back(0.0f);
    pointList.push_back(Vertex2F(0.0f, 0.0f));
    float delay = m_pRand_->next();
    delay = delay * delaySigma_ * 2.0f + delayMean_ - delaySigma_;
    delay = delay / 10.0f;
    pointDelayBuf.push_back(delay);
    for (int i = 1; i <= pointNumber_; i++)
    {
        float rand = m_pRand_->next();
        Assert(pointList.size() >= 2);
        Vertex2F dir = pointList[pointList.size() - 1] - pointList[pointList.size() - 2];
        dir.normalize();
        float len = dir.getLength();
        if (len == 0.0f)
        {
            rand = rand * 2 * PI;
            dir = Vertex2F(cos(rand), sin(rand));
        }
        else
        {
            rand = rand / directionRange_ * 2 * PI - PI / directionRange_;
            dir.rotate(rand);
        }
        delay = m_pRand_->next();//(0, 1)
        delay = delay * delaySigma_ * 2.0f + delayMean_ - delaySigma_;//(main - sigma, main + sigma)
        delay = (delay + i) / 10.0f;//real time and scale
        if (delay < pointDelayBuf[pointDelayBuf.size() - 1])
        {
            delay = pointDelayBuf[pointDelayBuf.size() - 1];
        }
        pointList.push_back(pointList[pointList.size() - 1] + dir);
        pointDelayBuf.push_back(delay);
        Assert(pointList.size() == pointDelayBuf.size());
    }

    /* Simulate Normal Communication */
    {
        float gameTime = 0.0f;
        Vertex2F serverPos;
        Vertex2F position;
        std::vector<Vertex2F>::iterator itPos = pointList.begin() + 1;
        size_t counter = 0;
        itPointList = pointList.begin() + 1;
        itPointDelay = pointDelayBuf.begin() + 1;
        while (itPointList != pointList.end())
        {
            /* Chase */
            Vertex2F dir = serverPos - position;
            float len = dir.getLength();
            if (len >= 0.2)
            {
                dir.normalize();
                dir = dir * 0.2;
                if (accelVelocity_)
                {
                    dir = dir * sqrt(log10(len * 5) + 1);
                }
            }
            position = position + dir;

            /* Input & Output */
            while (itPointDelay != pointDelayBuf.end())
            {
                float dt = *itPointDelay - gameTime;
                if (isAndSetZero(dt) || dt < 0)
                {
                    serverPos = *itPos;
                    itPointDelay++;
                    itPos++;
                    continue;
                }
                break;
            }
            if (counter % 5 == 0)
            {
                m_pServerList_->push_back(*itPointList);
                m_pClientServerList_->push_back(serverPos);
                m_pClientPositionList_->push_back(position);
                itPointList++;
            }
            gameTime += 0.02f;
            counter++;
        }
    }
    inited = true;
}

void PW::GenerateSyncPosition::reset()
{
    if (m_pServerList_ != nullptr) {
        Assert(m_pClientServerList_ != nullptr);
        Assert(m_pClientPositionList_ != nullptr);
        delete m_pServerList_;
        delete m_pClientServerList_;
        delete m_pClientPositionList_;
    }
    inited = false;
}

std::vector<PW::Vertex2F> PW::GenerateSyncPosition::getServerList()
{
    if (m_pServerList_ != nullptr)
    {
        Assert(m_pClientServerList_ != nullptr);
        Assert(m_pClientPositionList_ != nullptr);
        return *m_pServerList_;
    }
}

std::vector<PW::Vertex2F> PW::GenerateSyncPosition::getClientServerList()
{
    if (m_pClientServerList_ != nullptr)
    {
        Assert(m_pServerList_ != nullptr);
        Assert(m_pClientPositionList_ != nullptr);
        return *m_pClientServerList_;
    }
}

std::vector<PW::Vertex2F> PW::GenerateSyncPosition::getClientPosition()
{
    if (m_pClientPositionList_ != nullptr)
    {
        Assert(m_pServerList_ != nullptr);
        Assert(m_pClientServerList_ != nullptr);
        return *m_pClientPositionList_;
    }
}
