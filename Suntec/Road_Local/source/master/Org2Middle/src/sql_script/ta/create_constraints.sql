
-------------------------------------------------------------------------------------------------
-- link & node
-------------------------------------------------------------------------------------------------

CREATE INDEX nw_f_jnctid_idx
  ON org_nw
  USING btree
  (f_jnctid);

CREATE UNIQUE INDEX nw_id_idx
  ON org_nw
  USING btree
  (id);

CREATE UNIQUE INDEX nw_id_type_idx
  ON org_nw
  USING btree
  (cast(id as bigint));
  
CREATE INDEX nw_t_jnctid_idx
  ON org_nw
  USING btree
  (t_jnctid);
  
CREATE INDEX sr_id_idx
  ON org_sr
  USING btree
  (id);
  
CREATE INDEX ta_areid_idx
  ON org_ta
  USING btree
  (areid);

CREATE INDEX ta_id_idx
  ON org_ta
  USING btree
  (id);

CREATE INDEX ta_aretyp_idx
  ON org_ta
  USING btree
  (aretyp);
  
CREATE INDEX ae_id_idx
  ON org_ae
  USING btree
  (id);
  
CREATE UNIQUE INDEX temp_jc_id_idx
  ON temp_jc
  USING btree
  (id);

CREATE INDEX rs_id_idx
  ON org_rs
  USING btree
  (id);
  
CREATE INDEX temp_towardname_si_id_idx
  ON temp_towardname_si
  USING btree
  (id);
  
CREATE UNIQUE INDEX sg_id_idx
  ON org_sg
  USING btree
  (id);
-----------------------------------------
-- For rn
CREATE INDEX rn_id_idx
  ON org_rn
  USING btree
  (id);
CREATE INDEX rn_rteprior_idx
  ON org_rn
  USING btree
  (rteprior);
-----------------------------------------
-- For gc
CREATE INDEX gc_id_idx
  ON org_gc
  USING btree
  (id);
CREATE INDEX gc_nametyp_idx
  ON org_gc
  USING btree
  (nametyp);
 
-----------------------------------------
-- For jc  
CREATE INDEX org_jc_geom_idx
  ON org_jc
  USING gist
  (the_geom);
CREATE INDEX org_jc_elev_idx
  ON org_jc
  USING btree
  (elev);
CREATE INDEX org_jc_id_idx
  ON org_jc
  USING btree
  (id);
-----------------------------------------
-- For pi  
CREATE INDEX org_mn_pi_geom_idx
  ON org_mn_pi
  USING gist
  (the_geom);

CREATE INDEX org_mn_pi_feattyp_idx
  ON org_mn_pi
  USING btree
  (feattyp); 
  
CREATE INDEX org_mnpoi_pi_geom_idx
  ON org_mnpoi_pi
  USING gist
  (the_geom);

CREATE INDEX org_mnpoi_pi_feattyp_idx
  ON org_mnpoi_pi
  USING btree
  (feattyp);   
-----------------------------------------
-- For temp_mid_link_route_number  
CREATE INDEX temp_mid_link_route_number_link_id_idx
  ON temp_mid_link_route_number
  USING btree
  (link_id);
 
-----------------------------------------
-- For temp_mid_link_shield_link 
CREATE INDEX temp_mid_link_shield_link_id_idx
  ON temp_mid_link_shield
  USING btree
  (link_id);
  
-----------------------------------------
-- For temp_mid_link_name   
CREATE INDEX temp_mid_link_name_link_id_idx
  ON temp_mid_link_name
  USING btree
  (link_id);

-----------------------------------------
-- For temp_mid_link_route_number     
CREATE INDEX temp_mid_link_route_number_link_id_idx
  ON temp_mid_link_route_number
  USING btree
  (link_id);
  
-----------------------------------------
-- For temp_link_pos_cond_speed     
CREATE INDEX temp_link_pos_cond_speed_id_idx
  ON temp_link_pos_cond_speed
  USING btree
  (id);
  
-----------------------------------------
-- For temp_link_neg_cond_speed     
CREATE INDEX temp_link_neg_cond_speed_id_idx
  ON temp_link_neg_cond_speed
  USING btree
  (id);
  
-----------------------------------------
-- For jv        
CREATE INDEX jv_f_trpelid_mnl_idx
  ON org_jv
  USING btree
  (f_trpelid_mnl);

CREATE INDEX jv_id_idx
  ON org_jv
  USING btree
  (id);

CREATE INDEX jv_jnctid_mnl_idx
  ON org_jv
  USING btree
  (jnctid_mnl);

CREATE INDEX jv_t_trpelid_mnl_idx
  ON org_jv
  USING btree
  (t_trpelid_mnl);
  
CREATE INDEX temp_lane_link_info_id_idx
  ON temp_lane_link_info
  USING btree
  (id);
  
CREATE INDEX temp_lane_link_node_id_idx
  ON temp_lane_link_node
  USING btree
  (id);
  
CREATE INDEX org_ln_id_idx
  ON org_ln
  USING btree
  (id);
  
CREATE INDEX org_lp_id_idx
  ON org_lp
  USING btree
  (id);
  
CREATE INDEX org_ld_id_validity_idx
  ON org_ld
  USING btree
  (id,validity);

CREATE INDEX org_lf_id_validity_idx
  ON org_lf
  USING btree
  (id,validity);
  
CREATE INDEX org_ld_id_idx
  ON org_ld
  USING btree
  (id);

CREATE INDEX org_lf_id_idx
  ON org_lf
  USING btree
  (id);
  
-------------------------------------------------------------
CREATE INDEX mid_vm_phoneme_relation_id_idx
  ON mid_vm_phoneme_relation
  USING btree
  (id);

-------------------------------------------------------------
CREATE INDEX mid_vm_phoneme_relation_ptid_idx
  ON mid_vm_phoneme_relation
  USING btree
  (ptid);
  
-------------------------------------------------------------
CREATE INDEX org_vm_ne_ptid_idx
  ON org_vm_ne
  USING btree
  (ptid);
 
-------------------------------------------------------------
CREATE INDEX fki_nefa_foa
  ON org_vm_nefa
  USING btree
  (featdsetid, featsectid, featlayerid, 
  featitemid, featcat, featclass);

-------------------------------------------------------------
CREATE INDEX fki_nefa_ne
  ON org_vm_nefa
  USING btree
  (featdsetid, featsectid, 
  featlayerid, nameitemid);
  
-------------------------------------------------------------
CREATE INDEX org_vm_nefa_featclass_idx
  ON org_vm_nefa
  USING btree
  (featclass);
  
-------------------------------------------------------------
CREATE INDEX org_vm_pt_ptid_idx
  ON org_vm_pt
  USING btree
  (ptid);
  
-------------------------------------------------------------
CREATE INDEX org_vm_pt_ptid_lanphonset_idx
  ON org_vm_pt
  USING btree
  (ptid, lanphonset);

-------------------------------------------------------------
CREATE INDEX temp_adminid_newandold_id_old_idx
  ON temp_adminid_newandold
  USING btree
  (id_old);
  
CREATE INDEX temp_adminid_newandold_id_new_idx
  ON temp_adminid_newandold
  USING btree
  (id_new);
  
CREATE INDEX temp_adminid_newandold_level_idx
  ON temp_adminid_newandold
  USING btree
  (level);
-------------------------------------------------------------
CREATE INDEX scpoint_ext_featid_idx
  ON scpoint_ext
  USING btree
  (featid);
  
CREATE INDEX scpoint_ext_ll_featid_idx
  ON scpoint_ext_ll
  USING btree
  (featid);
  
---------------------------------------------------
CREATE INDEX org_ta_id_idx
  ON org_ta
  USING btree
  (id);
  
CREATE INDEX temp_school_zone_a0_id_idx
  ON temp_school_zone_a0
  USING btree
  (id);

CREATE INDEX temp_school_zone_s_e_node_id_idx
  ON temp_school_zone_s_e_node
  USING btree
  (id);
  
CREATE INDEX temp_school_zone_s_e_node_s_node_idx
  ON temp_school_zone_s_e_node
  USING btree
  (s_node);
  
CREATE INDEX temp_school_zone_s_e_node_e_node_idx
  ON temp_school_zone_s_e_node
  USING btree
  (e_node);
  
CREATE INDEX temp_school_zone_s_e_node_speed_idx
  ON temp_school_zone_s_e_node
  USING btree
  (speed);
  
CREATE INDEX temp_school_zone_guide_id_id_idx
  ON temp_school_zone_guide_id
  USING btree
  (id);
----------------------------------------------
CREATE INDEX org_mnpoi_pi_cltrpelid_idx
	ON org_mnpoi_pi
	USING btree
	(cltrpelid);
	
CREATE INDEX temp_poi_name_poi_id_idx
	ON temp_poi_name
	USING btree
	(poi_id);
	
CREATE INDEX temp_poi_logmark_id_idx
	ON temp_poi_logmark
	USING btree
	(id);
	
CREATE INDEX temp_mnpoi_logmark_id_idx
	ON temp_mnpoi_logmark
	USING btree
	(id);

CREATE INDEX temp_mn_logmark_id_idx
	ON temp_mn_logmark
	USING btree
	(id);
	
CREATE INDEX org_poi_pt_ptid_idx
  ON org_poi_pt
  USING btree
  (ptid);
----------------------------------------------
CREATE INDEX temp_mid_iso_country_code_idx
	ON temp_mid_iso_country_code
	USING btree
	(iso_country_num);
	
CREATE INDEX	temp_poi_category_mapping_org_id1_org_id2_idx
  ON temp_poi_category_mapping
  USING btree
  (org_id1,org_id2);
  
-----------------------------------------------
CREATE INDEX org_a0_id_idx
	ON org_a0
	USING btree
	(id);
CREATE INDEX org_a0_order00_idx
	ON org_a0
	USING btree
	(order00);
	
CREATE INDEX org_a1_id_idx
	ON org_a1
	USING btree
	(id);
CREATE INDEX org_a1_order00_idx
	ON org_a1
	USING btree
	(order00);
CREATE INDEX org_a1_order01_idx
	ON org_a1
	USING btree
	(order01);

CREATE INDEX org_a7_id_idx
	ON org_a7
	USING btree
	(id);
CREATE INDEX org_a7_order00_idx
	ON org_a7
	USING btree
	(order00);
CREATE INDEX org_a7_order01_idx
	ON org_a7
	USING btree
	(order01);
CREATE INDEX org_a7_order07_idx
	ON org_a7
	USING btree
	(order07);
	
CREATE INDEX org_a8_id_idx
	ON org_a8
	USING btree
	(id);
CREATE INDEX org_a8_order00_idx
	ON org_a8
	USING btree
	(order00);
CREATE INDEX org_a8_order01_idx
	ON org_a8
	USING btree
	(order01);
CREATE INDEX org_a8_order07_idx
	ON org_a8
	USING btree
	(order07);	
CREATE INDEX org_a8_order08_idx
	ON org_a8
	USING btree
	(order08);
CREATE INDEX mid_temp_admin_name_admin_id_idx
	ON mid_temp_admin_name
	USING btree
	(admin_id);			
-------------------------------------------------------
CREATE INDEX mid_temp_hwy_ic_out_nearby_poi_the_geom_idx
  ON mid_temp_hwy_ic_out_nearby_poi
  USING gist
  (the_geom);
-------------------------------------------------------
CREATE INDEX mid_temp_hwy_ic_out_nearby_poi_poi_id_idx
  ON mid_temp_hwy_ic_out_nearby_poi
  USING btree
  (poi_id);