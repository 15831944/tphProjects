# -*- coding: UTF8 -*-
#!/usr/bin/python
'''
Created on 2015-6-19

@author: wushengbing
'''
import component.component_base
from string import *

class comp_guideinfo_lane_zenrin(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_Lane')
        
    def _Do(self):
        
        self._makeGeneralLane()
        self._makeHwLane()
        self._makeLane()
        
        
        
    def _makeGeneralLane(self):
        self.log.info('make general lane...')
        self.CreateFunction2('zenrin_findpasslink')
        self.CreateFunction2('zenrin_findpasslink_count')
        self.CreateFunction2('zenrin_make_lanenum_lr')
        self.CreateTable2('temp_lane_tbl')
        
        ##左右附加车线
        self.log.info('make temp_lane_numlr...')
        sqlcmd = '''
                drop table if exists temp_lane_numlr;
                create table temp_lane_numlr
                as
                (
                    select  meshcode, tnodeno, snodeno, enodeno,
                            (zenrin_make_lanenum_lr(addinfo))[1] as lane_numl,
                            (zenrin_make_lanenum_lr(addinfo))[2] as lane_numr
                    from
                    (
                        select meshcode, tnodeno, snodeno, enodeno,array_agg(flanecd) as addinfo
                        from 
                        (
                            select distinct meshcode,tnodeno,snodeno , enodeno,laneno,flanecd
                            from org_lane
                            order by meshcode,tnodeno,snodeno , enodeno,laneno
                        ) as a1
                        group by meshcode, tnodeno, snodeno, enodeno
                    ) as b1
                     
                    order by meshcode, tnodeno, snodeno, enodeno,lane_numl,lane_numr
                );
        
            '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_lane_numlr_meshcode_tnodeno_snodeno_enodeno_idx')
        
        
        ##计算passlink
        self.log.info('make temp_lane_passlink...')
        sqlcmd = '''
            drop table if exists temp_lane_passlink;
            create table temp_lane_passlink
            as
            (
                select meshcode, tnodeno, snodeno, enodeno, ysnodeno, yenodeno,passlink,
                       zenrin_findpasslink_count(passlink,'|') as passlink_cnt
                from 
                (
                    select  bb.*,
                            zenrin_findpasslink(bb.meshcode, bb.tnodeno,bb.ysnodeno,bb.passageno_array) as passlink
                    from 
                    (
                        SELECT meshcode, tnodeno, snodeno, enodeno, ysnodeno, yenodeno, 
                               array_agg(passageno) as passageno_array
                        FROM
                        (
                            select * from  org_lane_node
                            order by gid
                        ) as a
                        group by  meshcode, tnodeno, snodeno, enodeno, ysnodeno, yenodeno
                        
                    ) as bb 
                    
                ) as cc 
                
                order by meshcode, tnodeno, snodeno, enodeno, ysnodeno, yenodeno,passlink,passlink_cnt
            ); 
        
            '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_lane_passlink_meshcode_tnodeno_snodeno_enodeno_ysnodeno_yenodeno_idx')
        
        
        ##作成车线信息和箭头信息
        self.log.info('make temp_lane_lane_arrow_info...')
        sqlcmd = '''
            drop table if exists temp_lane_lane_arrow_info;
            create table temp_lane_lane_arrow_info
            as
            (
                select  a.meshcode,a.tnodeno,a.snodeno,a.enodeno,a.ysnodeno,a.yenodeno,
                        (cast( sum(1<< (tlanesu - laneno)) as bit(16)))::varchar as laneinfo,
                        (case   when cast(ypaint as integer) = 0 then 1
                                when cast(ypaint as integer) > 0 and cast(ypaint as integer) < 90 then 2
                                when cast(ypaint as integer) = 90 then 4
                                when cast(ypaint as integer) > 90 and cast(ypaint as integer) < 180 then 8
                                when cast(ypaint as integer) = 180 then 2048
                                when cast(ypaint as integer) = -180 then 16
                                when cast(ypaint as integer) > -180 and cast(ypaint as integer) < -90 then 32
                                when cast(ypaint as integer) = -90 then 64
                                when cast(ypaint as integer) > -90 and cast(ypaint as integer) < 0 then 128
                                else 0 end ) as arrowinfo,
                        (array_agg(tlanesu))[1] as tlane_count 
                from org_lane as a
                group by a.meshcode,a.tnodeno,a.snodeno,a.enodeno,a.ysnodeno,a.yenodeno,a.ypaint
                order by a.meshcode,a.tnodeno,a.snodeno,a.enodeno,a.ysnodeno,a.yenodeno,a.ypaint
            );
            
        '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_lane_lane_arrow_info_meshcode_tnodeno_snodeno_enodeno_idx')
        self.CreateIndex2('temp_lane_lane_arrow_info_meshcode_tnodeno_snodeno_enodeno_ysnodeno_yenodeno_idx')      
        
        
        sqlcmd = ''' 
            insert into temp_lane_tbl(nodeid,inlinkid,outlinkid,passlid,passlink_cnt,
                                        lanenum,laneinfo,arrowinfo,lanenuml,lanenumr)
            (
            select  tnd.node_id,tlk1.link_id, tlk2.link_id, b.passlink,
                    (case when b.passlink_cnt is null then 0
                          else b.passlink_cnt end ) as passlink_cnt,
                   a.tlane_count as lanenum, 
                   substring(a.laneinfo,17-a.tlane_count,a.tlane_count) as laneinfo,
                   a.arrowinfo, c.lane_numl, c.lane_numr
            
            from  temp_lane_lane_arrow_info as a
            left join temp_lane_passlink as b
            using(meshcode,tnodeno,snodeno,enodeno,ysnodeno,yenodeno)
            
            left join temp_lane_numlr as c
            using(meshcode,tnodeno,snodeno,enodeno)
            
            left join org_road as d
                   on ( a.snodeno =d.snodeno and a.enodeno = d.enodeno
                       or 
                       a.enodeno =d.snodeno and a.snodeno = d.enodeno
                       )
                       and a.meshcode = d.meshcode
            left join temp_link_mapping as tlk1
            on tlk1.meshcode = d.meshcode and tlk1.linkno = d.linkno
                   
            left join org_road as e
               on ( a.ysnodeno = e.snodeno and a.yenodeno = e.enodeno
                    or 
                    a.yenodeno =e.snodeno and a.ysnodeno = e.enodeno
                   )
                   and a.meshcode = e.meshcode
            left join temp_link_mapping as tlk2
            on tlk2.meshcode = e.meshcode and tlk2.linkno = e.linkno
            
            left join temp_node_mapping as tnd
            on tnd.meshcode = a.meshcode and tnd.nodeno = a.tnodeno    
    
            order by tnd.node_id, tlk1.link_id, tlk2.link_id, b.passlink, a.tlane_count,
                     a.laneinfo, a.arrowinfo, c.lane_numl, c.lane_numr
        );
        '''
          
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
     
     
    def _makeHwLane(self):
        self.log.info('make highway lane...')
        self._makeHWLane_branch()
        self._makeHWLane_other()
        
        sqlcmd = '''
            insert into temp_lane_tbl(nodeid,inlinkid,outlinkid,passlid,passlink_cnt,
                                        lanenum,laneinfo,arrowinfo,lanenuml,lanenumr)
            (
                select tnd.node_id, tlk1.link_id, tlk2.link_id, a.passlid, a.passlink_cnt, 
                       a.lanenum, a.laneinfo, a.arrowinfo, a.lanenuml, a.lanenumr  
                from temp_hwlane_tbl as a
                left join temp_node_mapping as tnd
                on tnd.meshcode = a.meshcode and tnd.nodeno = a.nodeid
                left join temp_link_mapping as tlk1
                on tlk1.meshcode = a.meshcode and tlk1.linkno = a.inlinkid
                left join temp_link_mapping as tlk2
                on tlk2.meshcode = a.meshcode and tlk2.linkno = a.outlinkid
                
                order by tnd.node_id, tlk1.link_id, tlk2.link_id, a.passlid, a.passlink_cnt, 
                       a.lanenum, a.laneinfo, a.arrowinfo, a.lanenuml, a.lanenumr
            
            )
        
        '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        
    def _makeLane(self):
        
        self.CreateTable2('lane_tbl')
        
        sqlcmd = '''
              insert into lane_tbl(id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                                            lanenum, laneinfo, arrowinfo, lanenuml, lanenumr,buslaneinfo)
              (
                  select gid, nodeid, inlinkid, outlinkid, passlid, passlink_cnt,lanenum, laneinfo, arrowinfo, 
                  lanenuml, lanenumr,'0000000000000000'
                  from temp_lane_tbl
                
              )    
        
             '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
                
           
    def _makeHWLane_branch(self):
        
        self.log.info('make highway lane  branch...')       
        self.CreateTable2('temp_hwlane_tbl')
        
        sqlcmd='''
        select meshcode,sroadno,slaneinf,array_agg(elaneinf) as elaneinf_arr,
        array_agg(eroadno) as eroadno_arr,array_agg(tnodeno) as tnodeno_arr 
        from org_hwlane a
        join
        (
            select meshcode,sroadno from org_hwlane a join
            (
                select meshcode,sroadno from org_hwlane group by meshcode,sroadno having count(1)=2
            ) b
            using(meshcode,sroadno)
            group by meshcode,sroadno
            having count(distinct(slaneinf))=1
        ) b
        using(meshcode,sroadno)
        group by meshcode,sroadno,slaneinf
        '''
        
        sqlcmd_insert = '''
            insert into temp_hwlane_tbl(nodeid,meshcode, inlinkid,outlinkid,passlid,passlink_cnt,lanenum,
                                        laneinfo,arrowinfo,lanenuml,lanenumr)
            values(%d,%s,%d,%d,'',0,%d,%s,%d,%d,%d)
            '''
        self.pg.execute(sqlcmd)
        results = self.pg.fetchall2()
        for result in results:
            meshcode = result[0]
            sroadno = result[1]
            slaneinf = result[2]
            elaneinf_1 = result[3][0]
            elaneinf_2 = result[3][1]
            eroadno_1 = result[4][0]
            eroadno_2 = result[4][1]
            if eroadno_1 == eroadno_2:
#                print 'error'
                continue
            if result[5][0]<>result[5][1]:
#                print 'tnodeno error!'
                continue
            tnodeno=result[5][0]
            slaneinf_l=slaneinf.find('1')+1
            slaneinf_r=slaneinf.rfind('1')+1
            elaneinf_all=rjust(str(int(elaneinf_1)+int(elaneinf_2)),16,'0')
            
#            if elaneinf_all.strip('0').find('0')<>-1:
#                continue
            
            elaneinf_l = 17 - len(elaneinf_all.lstrip('0'))
            elaneinf_r = len(elaneinf_all.rstrip('0'))
            lanenum = elaneinf_r - elaneinf_l+1
            
            lanenum_l = 0
            lanenum_r = 0
                      
            slaneinfo_1 = elaneinf_1[elaneinf_l-1:elaneinf_r]
            slaneinfo_2 = elaneinf_2[elaneinf_l-1:elaneinf_r]
            

            arrow = 0
                       
            self.pg.execute(sqlcmd_insert%(tnodeno,meshcode,sroadno,eroadno_1,lanenum,slaneinfo_1,arrow,lanenum_l,lanenum_r))

            self.pg.execute(sqlcmd_insert%(tnodeno,meshcode,sroadno,eroadno_2,lanenum,slaneinfo_2,arrow,lanenum_l,lanenum_r))
            self.pg.commit2()
                
    def _makeHWLane_other(self):
        
        self.log.info('make highway lane others...')
        sqlcmd='''
            select meshcode, sroadno, slaneinf, array_agg(elaneinf) as elaneinf_arr,
                   array_agg(eroadno) as eroadno_arr, array_agg(tnodeno) as tnodeno_arr 
            from org_hwlane as a  
            group by meshcode, sroadno, slaneinf having count(1)=1
              
            ''' 
        
        sqlcmd_insert = '''
            insert into temp_hwlane_tbl(nodeid,meshcode, inlinkid,outlinkid,passlid,passlink_cnt,lanenum,laneinfo,arrowinfo,lanenuml,lanenumr)
            values(%d,%s,%d,%d,'',0,%d,%s,%d,%d,%d)
            '''
        self.pg.execute(sqlcmd)
        results = self.pg.fetchall2()        

        for result in results:
            meshcode = result[0]
            sroadno = result[1]
            slaneinf = result[2]
            elaneinf = result[3][0]
            eroadno = result[4][0]
            tnodeno = result[5][0]
            
            slaneinf_l = slaneinf.find('1') + 1
            slaneinf_r = slaneinf.rfind('1') + 1
            
            if slaneinf_r == slaneinf_l:
                lanenum = 1
            else:
                lanenum = slaneinf_r - slaneinf_l + 1
            
            slaneinfo = slaneinf[slaneinf_l-1:slaneinf_r]

          
            lanenum_l = 0
            lanenum_r = 0 
            arrow = 0    
            self.pg.execute(sqlcmd_insert%(tnodeno,meshcode,sroadno,eroadno,lanenum,slaneinfo,arrow,lanenum_l,lanenum_r))
            self.pg.commit2()
        
        self.CreateIndex2('temp_hwlane_tbl_meshcode_idx')
        self.CreateIndex2('temp_hwlane_tbl_inlinkid_idx')
        self.CreateIndex2('temp_hwlane_tbl_outlinkid_idx')
        
        

 
        
   
        
        
        
        
        
