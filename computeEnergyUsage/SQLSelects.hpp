#include <string>
#include <string.h>
using namespace std;
extern const char *ptrFallWinterAndSpringSeasons;
extern const char *ptrSummerOnlySeason;
extern const char *ptrSummerOnlySeasonSubSelect;
extern const char *ptrSummerOnlySeasonSubSubSelect;
extern const char *ptrSummerOnlySeasonSubSubSubSelect;
extern const string strFallWinterAndSpringSeasons;
extern const string strSummerOnlySeason;
extern const string strSummerOnlySeasonSubSelect;
extern const string strSummerOnlySeasonSubSubSelect;
extern const string strSummerOnlySeasonSubSubSubSelect;
extern const int szOfptrFallWinterAndSpringSeasons;
extern const int szOfptrSummerOnlySeason;
extern const int szOfptrSummerOnlySeasonSubSubSelect;
extern const int szOfptrSummerOnlySeasonSubSubSubSelect;
#define FALLWINTERSPRING 1
#define SUMMERONLYSEASON 2
class SQLSelects {
private:
//
//-- M2 energy usage during fall, winter and spring seasons 
//--
const char *ptrFallWinterAndSpringSeasons = \
"SELECT e.sumconv/e.sumcnt AS \"M2 NOT Summer Energy Usage\" FROM ( \
    SELECT sum(d.convolution) AS sumconv, sum(d.counts) AS sumcnt FROM (\
        SELECT c.m2 * c.cnt AS convolution, c.cnt AS counts FROM (\
            SELECT b.m2kwh-a.m2kwh AS m2, count(*) as cnt FROM ( \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) a \
                INNER JOIN ( \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) b \
                ON date_part('J', a.date)+1 = date_part('J', b.date) WHERE \
                    date_part('DOY',b.date) NOT BETWEEN (151) AND (273) \
                        GROUP BY (b.m2kwh - a.m2kwh) \
                        ORDER BY (b.m2kwh-a.m2kwh) ) c ) d) e;";
const int szOfptrFallWinterAndSpringSeasons = sizeof(ptrFallWinterAndSpringSeasons)/sizeof(ptrFallWinterAndSpringSeasons[0]);
//--
//-- M2 energy usage during summer season
//-- Note that we care only about year 2018 because previous years (2015, 2016, and 2017) the
//-- Oasis Basement dehumidifier -- purchased in May of 2013, before we kept detailed energy 
//-- usage records -- kept running and just consumed a lot of electricity without taking water
//-- out of the basement and, therefore, lowering the basement's humidity. In March of 2018
//-- I replaced this Oasis dehumidifier (cost $2000!) with the latest model which lowers the 
//-- basement's humidifier and uses much less electricity. Therefore, it makes no sesnse to 
//-- include previous years' M2 energy usage (the dehumidifier draws power from electric meter
//-- M2), which will only pollute our model with unreasonable faulty data, for predicting future
//-- M2 energy usage. 
//-- NB: We define summer season, when dehumidifer is in use, as lasting from 
//-- June 1st (day-of-year = 151) to September 30th (day-of-year = 273) -- 
//-- and NO we don't care about leap years since the added complexity does
//-- not overcome the trivial influence upon the predicted energy usage model.
//--
const char *ptrSummerOnlySeason = \
"SELECT e.sumconv/e.sumcnt AS \"M2 Summer Energy Usage\" FROM ( \
    SELECT sum(d.convolution) AS sumconv, sum(d.counts) AS sumcnt FROM ( \
        SELECT c.m2 * c.cnt AS convolution, c.cnt AS counts FROM ( \
            SELECT b.m2kwh-a.m2kwh AS m2, count(*) as cnt FROM ( \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) a \
                INNER JOIN ( \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) b \
                ON date_part('J', a.date)+1 = date_part('J', b.date) WHERE \
                    (date_part('DOY',b.date)  BETWEEN (151) AND (273)) AND (date_part('Year', b.date) > (2017))  \
                    GROUP BY (b.m2kwh - a.m2kwh) \
                    ORDER BY (b.m2kwh-a.m2kwh) ) c ) d) e;";
const int szOfptrSummerOnlySeason = sizeof(ptrSummerOnlySeason);
//--
//--
//-- Take away e to look at sub-select for gathering the quotient of the sum of the convolution divided by the sum of the counts
//-- SELECT e.sumconv/e.sumcnt FROM (
const char *ptrSummerOnlySeasonSubSelect = \
"    SELECT sum(d.convolution) AS sumconv, sum(d.counts) AS sumcnt FROM ( \
        SELECT c.m2 * c.cnt AS convolution, c.cnt AS counts FROM ( \
            SELECT b.m2kwh-a.m2kwh AS m2, count(*) as cnt FROM ( \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) a \
                INNER JOIN (\
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) b \
                ON date_part('J', a.date)+1 = date_part('J', b.date) WHERE \
                    (date_part('DOY',b.date)  BETWEEN (151) AND (273)) AND (date_part('Year', b.date) > (2017))  \
                    GROUP BY (b.m2kwh - a.m2kwh) \
                    ORDER BY (b.m2kwh-a.m2kwh) ) c ) d;"; //-- ) e;
const int szOfptrSummerOnlySeasonSubSelect = sizeof(ptrSummerOnlySeasonSubSelect);
//--
//--
//--
//-- Take away d and e to look at the sub-sub select for gathering the details of: 
//--      i) the convolution of the m2 energy usage times the conunts;
//--     ii) the counts;
//--    iii) the m2 energy usage.
//-- SELECT e.sumconv/e.sumcnt FROM (
//--    SELECT sum(d.convolution) AS sumconv, sum(d.counts) AS sumcnt FROM (
const char *ptrSummerOnlySeasonSubSubSelect = \
"        SELECT c.m2 * c.cnt AS convolution, c.cnt AS counts, c.m2 FROM ( \
            SELECT b.m2kwh-a.m2kwh AS m2, count(*) as cnt FROM (  \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) a  \
                INNER JOIN ( \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) b  \
                ON date_part('J', a.date)+1 = date_part('J', b.date) WHERE  \
                    (date_part('DOY',b.date)  BETWEEN (151) AND (273)) AND (date_part('Year', b.date) > (2017))   \
                    GROUP BY (b.m2kwh - a.m2kwh)  \
                    ORDER BY (b.m2kwh-a.m2kwh) ) c;"; // -- ) d -- ) e;
const int szOfptrSummerOnlySeasonSubSubSelect = sizeof(*ptrSummerOnlySeasonSubSubSelect);
//--
//--
//--
//-- Take away c, d, and e to look at the sub-sub-sub select for gathering the details of:
//--     i) the difference between yesterday's m2 energy usage and the day-before-yesterday's energy usage
//--    ii) the count of how many times we encountered this amount of energy usage.
//-- SELECT e.sumconv/e.sumcnt FROM (
//--    SELECT sum(d.convolution) AS sumconv, sum(d.counts) AS sumcnt FROM (
//--        SELECT c.m2 * c.cnt AS convolution, c.cnt AS counts, c.m2 FROM (
const char *ptrSummerOnlySeasonSubSubSubSelect = \
"            SELECT b.m2kwh-a.m2kwh AS m2, count(*) as cnt FROM (  \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) a  \
                INNER JOIN ( \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) b  \
                ON date_part('J', a.date)+1 = date_part('J', b.date) WHERE  \
                    (date_part('DOY',b.date)  BETWEEN (151) AND (273)) AND (date_part('Year', b.date) > (2017))   \
                    GROUP BY (b.m2kwh - a.m2kwh)  \
                    ORDER BY (b.m2kwh-a.m2kwh);"; // --) c; -- ) d -- ) e; 
const int szOfptrSummerOnlySeasonSubSubSubSelect = sizeof(ptrSummerOnlySeasonSubSubSelect);
//--
//-- M2 energy usage during fall, winter and spring seasons 
//--
const string strFallWinterAndSpringSeasons(const char *ptrFallWinterAndSpringSeasons);
/*
"SELECT e.sumconv/e.sumcnt AS \"M2 NOT Summer Energy Usage\" FROM (  \
    SELECT sum(d.convolution) AS sumconv, sum(d.counts) AS sumcnt FROM ( \
        SELECT c.m2 * c.cnt AS convolution, c.cnt AS counts FROM ( \
            SELECT b.m2kwh-a.m2kwh AS m2, count(*) as cnt FROM (  \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) a  \
                INNER JOIN ( \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) b  \
                ON date_part('J', a.date)+1 = date_part('J', b.date) WHERE  \
                    date_part('DOY',b.date) NOT BETWEEN (151) AND (273) \
                        GROUP BY (b.m2kwh - a.m2kwh)  \
                        ORDER BY (b.m2kwh-a.m2kwh) ) c ) d) e;");
 */
//--
//-- M2 energy usage during summer season
//-- Note that we care only about year 2018 because previous years (2015, 2016, and 2017) the
//-- Oasis Basement dehumidifier -- purchased in May of 2013, before we kept detailed energy 
//-- usage records -- kept running and just consumed a lot of electricity without taking water
//-- out of the basement and, therefore, lowering the basement's humidity. In March of 2018
//-- I replaced this Oasis dehumidifier (cost $2000!) with the latest model which lowers the 
//-- basement's humidifier and uses much less electricity. Therefore, it makes no sesnse to 
//-- include previous years' M2 energy usage (the dehumidifier draws power from electric meter
//-- M2), which will only pollute our model with unreasonable faulty data, for predicting future
//-- M2 energy usage. 
//-- NB: We define summer season, when dehumidifer is in use, as lasting from 
//-- June 1st (day-of-year = 151) to September 30th (day-of-year = 273) -- 
//-- and NO we don't care about leap years since the added complexity does
//-- not overcome the trivial influence upon the predicted energy usage model.
//--
const string strSummerOnlySeason (const char *ptrSummerOnlySeason); /*
"SELECT e.sumconv/e.sumcnt AS \"M2 Summer Energy Usage\" FROM ( \
    SELECT sum(d.convolution) AS sumconv, sum(d.counts) AS sumcnt FROM ( \
        SELECT c.m2 * c.cnt AS convolution, c.cnt AS counts FROM ( \
            SELECT b.m2kwh-a.m2kwh AS m2, count(*) as cnt FROM (  \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) a  \
                INNER JOIN ( \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) b  \
                ON date_part('J', a.date)+1 = date_part('J', b.date) WHERE  \
                    (date_part('DOY',b.date)  BETWEEN (151) AND (273)) AND (date_part('Year', b.date) > (2017))   \
                    GROUP BY (b.m2kwh - a.m2kwh)  \
                    ORDER BY (b.m2kwh-a.m2kwh) ) c ) d) e;"); */

//--
//--
//-- Take away e to look at sub-select for gathering the quotient of the sum of the convolution divided by the sum of the counts
//-- SELECT e.sumconv/e.sumcnt FROM (

const string strSummerOnlySeasonSubSelect (const char *ptrSummerOnlySeasonSubSelect);  /*
"    SELECT sum(d.convolution) AS sumconv, sum(d.counts) AS sumcnt FROM ( \
        SELECT c.m2 * c.cnt AS convolution, c.cnt AS counts FROM ( \
            SELECT b.m2kwh-a.m2kwh AS m2, count(*) as cnt FROM (  \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) a  \
                INNER JOIN ( \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) b  \
                ON date_part('J', a.date)+1 = date_part('J', b.date) WHERE  \
                    (date_part('DOY',b.date)  BETWEEN (151) AND (273)) AND (date_part('Year', b.date) > (2017))   \
                    GROUP BY (b.m2kwh - a.m2kwh)  \
                    ORDER BY (b.m2kwh-a.m2kwh) ) c ) d;"); //-- ) e; */
//--
//--
//--
//-- Take away d and e to look at the sub-sub select for gathering the details of: 
//--      i) the convolution of the m2 energy usage times the conunts;
//--     ii) the counts;
//--    iii) the m2 energy usage.
//-- SELECT e.sumconv/e.sumcnt FROM (
//--    SELECT sum(d.convolution) AS sumconv, sum(d.counts) AS sumcnt FROM (
const string strSummerOnlySeasonSubSubSelect (const char *ptrSummerOnlySeasonSubSubSelect);  /*
"        SELECT c.m2 * c.cnt AS convolution, c.cnt AS counts, c.m2 FROM ( \
            SELECT b.m2kwh-a.m2kwh AS m2, count(*) as cnt FROM (  \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) a  \
                INNER JOIN ( \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) b  \
                ON date_part('J', a.date)+1 = date_part('J', b.date) WHERE  \
                    (date_part('DOY',b.date)  BETWEEN (151) AND (273)) AND (date_part('Year', b.date) > (2017))   \
                    GROUP BY (b.m2kwh - a.m2kwh)  \
                    ORDER BY (b.m2kwh-a.m2kwh) ) c;"); // -- ) d -- ) e; */
//--
//--
//--
//-- Take away c, d, and e to look at the sub-sub-sub select for gathering the details of:
//--     i) the difference between yesterday's m2 energy usage and the day-before-yesterday's energy usage
//--    ii) the count of how many times we encountered this amount of energy usage.
//-- SELECT e.sumconv/e.sumcnt FROM (
//--    SELECT sum(d.convolution) AS sumconv, sum(d.counts) AS sumcnt FROM (
//--        SELECT c.m2 * c.cnt AS convolution, c.cnt AS counts, c.m2 FROM (
const string strSummerOnlySeasonSubSubSubSelect (const char *ptrSummerOnlySeasonSubSubSubSelect); /*
"            SELECT b.m2kwh-a.m2kwh AS m2, count(*) as cnt FROM (  \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) a  \
                INNER JOIN ( \
                SELECT date, time, m2kwh, date_part('DOY', date) FROM tbl_energy_usage order by date) b  \
                ON date_part('J', a.date)+1 = date_part('J', b.date) WHERE  \
                    (date_part('DOY',b.date)  BETWEEN (151) AND (273)) AND (date_part('Year', b.date) > (2017))   \
                    GROUP BY (b.m2kwh - a.m2kwh)  \
                    ORDER BY (b.m2kwh-a.m2kwh);"); // --) c; -- ) d -- ) e;" */
public:
    SQLSelects(void);
    ~SQLSelects(void);
    const char *returnSQLSelect(int);
};
