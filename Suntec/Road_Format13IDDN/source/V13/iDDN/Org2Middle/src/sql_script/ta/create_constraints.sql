
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
  
CREATE INDEX org_ln_id_idx
  ON org_ln
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
CREATE INDEX FKI_nefa_foa
  ON org_vm_nefa
  USING btree
  (featdsetid, featsectid, featlayerid, 
  featitemid, featcat, featclass);

-------------------------------------------------------------
CREATE INDEX FKI_nefa_ne
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
