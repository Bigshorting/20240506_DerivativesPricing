#include <iostream>
#include <cmath>
#include <random>

template<bool RANDOM_SEED = false, typename RANDOM_ENGINE = std::mt19937>
double random_nd() {
    static RANDOM_ENGINE gen(RANDOM_SEED ? std::random_device{}() : 0);
    static std::normal_distribution<double> dist(0.0, 1.0);

    return dist(gen);
}

/**
 * @brief 使用简单蒙特卡洛方法计算欧式期权价值
 *
 * 根据给定的到期时间、行权价格、标的资产价格、波动率、无风险利率和模拟路径数量，使用简单蒙特卡洛方法计算欧式期权价值。
 *
 * @param Expiry 到期时间
 * @param Strike 行权价格
 * @param Spot 标的资产价格
 * @param Return 标的收益率
 * @param Vol 波动率
 * @param r 无风险利率
 * @param NumberOfPath 模拟路径数量
 *
 * @return 计算得到的欧式期权价值
 */
template<bool Call = true, bool American = false>
double SimpleMonteCarlo(
        size_t Expiry,
        size_t Strike,
        size_t Spot,
        double Return,
        double Vol,
        double r,
        size_t NumberOfPath
) {
    double variance = Vol * Vol * Expiry; // 期权期间股价的方差
    double rootVariance = sqrt(variance); // 期权期间股价的标准差
    double itoCorrelation = -0.5 * variance; // 伊藤引理修正的股票收益率

    double movedSpot = Spot * exp((Return + r) * Expiry + itoCorrelation); // 期望收益率下的股价
    double thisSpot;
    double runningSum = 0;

    if (American) {
        for (size_t i = 0; i < NumberOfPath; ++i) {
            double spotPath[static_cast<int>(Expiry)];
            spotPath[0] = movedSpot;
            for (int j = 1; j < Expiry; ++j) {
                spotPath[j] = spotPath[j - 1] * exp(rootVariance * random_nd());
            }

            double thisPayoff = Call ? spotPath[Expiry - 1] - Strike : Strike - spotPath[Expiry - 1];
            thisPayoff = thisPayoff > 0 ? thisPayoff : 0;

            for (int j = Expiry - 2; j >= 0; --j) {
                double earlyExercise = Call ? spotPath[j] - Strike : Strike - spotPath[j];
                earlyExercise = earlyExercise > 0 ? earlyExercise : 0;
                thisPayoff = fmax(thisPayoff, earlyExercise * exp(-r * (Expiry - j)));
            }

            runningSum += thisPayoff;
        }
    } else {
        for (size_t i = 0; i < NumberOfPath; ++i) {
            thisSpot = movedSpot * exp(rootVariance * random_nd()); // 蒙特卡洛模拟下的股价
            double thisPayoff = Call ? thisSpot - Strike : Strike - thisSpot;
            thisPayoff = thisPayoff > 0 ? thisPayoff : 0; // 蒙特卡洛模拟下的期权收益
            runningSum += thisPayoff;
        }
    }

    double mean = runningSum / NumberOfPath;
    mean *= exp(-r * Expiry);
    return mean;
}

int main() {
    {
        double Expiry = 1;
        double Strike = 100;
        double Spot = 100;
        double Return = 0;
        double Vol = 0.3;
        double r = 0;

        double call_result = SimpleMonteCarlo(Expiry, Strike, Spot, Return, Vol, r, 1e7);
        double put_result = SimpleMonteCarlo<false>(Expiry, Strike, Spot, Return, Vol, r, 1e7);

        double call_parity = call_result + Strike * exp(-r * Expiry);
        double put_parity = put_result + Spot;

        std::cout << std::endl << "test Put-call Parity:" << std::endl;
        std::cout << "Call + K*B(r,t) == " << call_parity << std::endl;
        std::cout << "Put + S(t) == " << put_parity << std::endl;

        /**
test Put-call Parity:
Call + K*B(r,t) == 111.924
Put + S(t) == 111.926
         */
    }

    {
        double Expiry = 10;
        double Strike = 100;
        double Spot = 100;
        double Return = 0.005;
        double Vol = 0.03;
        double r = 0.003;

        double call_result = SimpleMonteCarlo<true>(Expiry, Strike, Spot, Return, Vol, r, 1e7);
        double put_result = SimpleMonteCarlo<false>(Expiry, Strike, Spot, Return, Vol, r, 1e7);

        double call_parity = call_result + Strike * exp(-r * Expiry);
        double put_parity = put_result + Spot;

        std::cout << std::endl << "test Put-call Parity:" << std::endl;
        std::cout << "Call + K*B(r,t) == " << call_parity << std::endl;
        std::cout << "Put + S(t) == " << put_parity << std::endl;
    }

    return 0;
}






