
CREATE TABLE temp_condition_regulation_tbl
(
  cond_id integer,
  day character varying(9),
  shour character varying(4),
  ehour character varying(4),
  sdate character varying(4),
  edate character varying(4),
  cartype integer
);

CREATE TABLE temp_org_one_way
(
  gid serial primary key,
  linkno bigint not null,
  dir smallint not null,
  day character varying(9),
  shour character varying(4),
  ehour character varying(4),
  sdate character varying(4),
  edate character varying(4),
  cartype integer
);

CREATE TABLE temp_org_not_in
(
  gid serial primary key,
  fromlinkno bigint not null,
  tolinkno bigint not null,
  snodeno bigint not null,
  tnodeno bigint not null,
  enodeno bigint not null,
  day character varying(9),
  shour character varying(4),
  ehour character varying(4),
  sdate character varying(4),
  edate character varying(4),
  cartype integer,
  carwait smallint,
  carthaba smallint,
  carthigh smallint
);