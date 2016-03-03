drop table if exists users cascade;
create table users(
id serial,
name character varying(32),
age integer,
constraint users_primary_key primary key(id)
);

drop table if exists users_0 cascade;
create table users_0 ( check(5>=age),
constraint users_0_primary_key primary key(id)
) inherits(users);
--users_1 10>=age>5 放入这个表
drop table if exists users_1 cascade;
create table users_1 (
check(10>=age and age >5),
constraint users_1_primary_key primary key(id)
) inherits(users);
--users_2 大于10 放在这个表中
drop table if exists users_2 cascade;
create table users_2 (
check(age>10),
constraint users_2_primary_key primary key(id)
) inherits(users);

-- users_0
create rule users_insert_0 as on insert to users where(age <= 5) do instead insert into users_0 values(NEW.id,NEW.name,NEW.age); 
-- users_1
create rule users_insert_1 as on insert to users where (age > 5 and age <=10) do instead insert into users_1 values(NEW.id,NEW.name,NEW.age);
-- users_2
create rule users_insert_2 as on insert to users where (age > 10)do instead insert into users_2 values(NEW.id,NEW.name,NEW.age);

drop table if exists users_0_0 cascade;
create table users_0_0 ( check(age<=3),
constraint users_0_0_primary_key primary key(id)
) inherits(users_0);

drop table if exists users_0_1 cascade;
create table users_0_1 ( check(3<age and age<=5),
constraint users_0_1_primary_key primary key(id)
) inherits(users_0);

create rule users_insert_0_0 as on insert to users_0 where(age<=3) do instead insert into users_0_0 values(NEW.id,NEW.name,NEW.age); 
create rule users_insert_0_1 as on insert to users_0 where(3<age and age<=5) do instead insert into users_0_1 values(NEW.id,NEW.name,NEW.age); 

insert into users(name,age) values('daigong1',1);
insert into users(name,age) values('daigong4',4);
insert into users(name,age) values('daigong6',6);
insert into users(name,age) values('daigong9',9);
insert into users(name,age) values('daigong100',100);
select * from users order by id;









