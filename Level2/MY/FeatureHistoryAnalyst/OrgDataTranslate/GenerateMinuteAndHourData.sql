-- ************************************************************************************************************************************************************************
-- 以分钟为单位将tick数据制作成分钟数据
drop table if exists rm01_minute;
select datet, substr(timet::varchar, 1, 5) as minutet, (array_agg(pricet))[1] as open_pricet,
	(array_agg(pricet))[array_upper(array_agg(pricet), 1)] as close_pricet,
	max(pricet) as highest_pricet, min(pricet) as lowest_pricet, sum(volt) as total_volumnt
into rm01_minute
from rm01
group by datet, substr(timet::varchar, 1, 5);

drop table if exists rm05_minute;
select datet, substr(timet::varchar, 1, 5) as minutet, (array_agg(pricet))[1] as open_pricet,
	(array_agg(pricet))[array_upper(array_agg(pricet), 1)] as close_pricet,
	max(pricet) as highest_pricet, min(pricet) as lowest_pricet, sum(volt) as total_volumnt
into rm05_minute
from rm05
group by datet, substr(timet::varchar, 1, 5);

drop table if exists rm07_minute;
select datet, substr(timet::varchar, 1, 5) as minutet, (array_agg(pricet))[1] as open_pricet,
	(array_agg(pricet))[array_upper(array_agg(pricet), 1)] as close_pricet,
	max(pricet) as highest_pricet, min(pricet) as lowest_pricet, sum(volt) as total_volumnt
into rm07_minute
from rm07
group by datet, substr(timet::varchar, 1, 5);

drop table if exists rm09_minute;
select datet, substr(timet::varchar, 1, 5) as minutet, (array_agg(pricet))[1] as open_pricet,
	(array_agg(pricet))[array_upper(array_agg(pricet), 1)] as close_pricet,
	max(pricet) as highest_pricet, min(pricet) as lowest_pricet, sum(volt) as total_volumnt
into rm09_minute
from rm09
group by datet, substr(timet::varchar, 1, 5);

drop table if exists rm11_minute;
select datet, substr(timet::varchar, 1, 5) as minutet, (array_agg(pricet))[1] as open_pricet,
	(array_agg(pricet))[array_upper(array_agg(pricet), 1)] as close_pricet,
	max(pricet) as highest_pricet, min(pricet) as lowest_pricet, sum(volt) as total_volumnt
into rm11_minute
from rm11
group by datet, substr(timet::varchar, 1, 5);

-- ************************************************************************************************************************************************************************
-- 以小时为单位将tick数据制作成小时数据
drop table if exists rm01_hour;
select datet, substr(timet::varchar, 1, 2) as hourt, (array_agg(pricet))[1] as open_pricet,
	(array_agg(pricet))[array_upper(array_agg(pricet), 1)] as close_pricet,
	max(pricet) as highest_pricet, min(pricet) as lowest_pricet, sum(volt) as total_volumnt
into rm01_hour
from rm01
group by datet, substr(timet::varchar, 1, 2);

drop table if exists rm05_hour;
select datet, substr(timet::varchar, 1, 2) as hourt, (array_agg(pricet))[1] as open_pricet,
	(array_agg(pricet))[array_upper(array_agg(pricet), 1)] as close_pricet,
	max(pricet) as highest_pricet, min(pricet) as lowest_pricet, sum(volt) as total_volumnt
into rm05_hour
from rm05
group by datet, substr(timet::varchar, 1, 2);

drop table if exists rm07_hour;
select datet, substr(timet::varchar, 1, 2) as hourt, (array_agg(pricet))[1] as open_pricet,
	(array_agg(pricet))[array_upper(array_agg(pricet), 1)] as close_pricet,
	max(pricet) as highest_pricet, min(pricet) as lowest_pricet, sum(volt) as total_volumnt
into rm07_hour
from rm07
group by datet, substr(timet::varchar, 1, 2);

drop table if exists rm09_hour;
select datet, substr(timet::varchar, 1, 2) as hourt, (array_agg(pricet))[1] as open_pricet,
	(array_agg(pricet))[array_upper(array_agg(pricet), 1)] as close_pricet,
	max(pricet) as highest_pricet, min(pricet) as lowest_pricet, sum(volt) as total_volumnt
into rm09_hour
from rm09
group by datet, substr(timet::varchar, 1, 2);

drop table if exists rm11_hour;
select datet, substr(timet::varchar, 1, 2) as hourt, (array_agg(pricet))[1] as open_pricet,
	(array_agg(pricet))[array_upper(array_agg(pricet), 1)] as close_pricet,
	max(pricet) as highest_pricet, min(pricet) as lowest_pricet, sum(volt) as total_volumnt
into rm11_hour
from rm11
group by datet, substr(timet::varchar, 1, 2);