create table temp_link_dummy_walked
(
    meshcode varchar(10),
    linkno   int
);

create table temp_dummy_link
(
    meshcode varchar(10),
    linkno int,
    snodeno int,
    enodeno int,
    oneway  integer,
    elcode  varchar(10),
    the_geom geometry
);