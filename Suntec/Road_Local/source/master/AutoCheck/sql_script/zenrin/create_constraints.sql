create index org_road_snodeno_enodeno_idx
on org_road
using btree
(snodeno,enodeno);

create index org_road_meshcode_linkno_idx
on org_road
using btree
(meshcode,linkno);