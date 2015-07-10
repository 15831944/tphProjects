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
        sqlcmd = ''' 
            insert into temp_lane_tbl(nodeid,inlinkid,outlinkid,passlid,passlink_cnt,
                                    lanenum,laneinfo,arrowinfo,lanenuml,lanenumr)
            (
            select  tnd.node_id,tlk1.link_id, tlk2.link_id, p.passlink,
                    (case when p.passlink_cnt is null then 0
                          else p.passlink_cnt end ) as passlink_cnt,
                   aa.tlane_count as lanenum, 
                   substring(aa.laneinfo,17-aa.tlane_count,aa.tlane_count) as laneinfo,
                   aa.arrowinfo, aa.lanenuml, aa.lanenumr  
                from
                (        
                    select  a.meshcode,a.tnodeno,a.snodeno,a.enodeno,a.ysnodeno,a.yenodeno,
                        (array_agg(lanesu))[1] lane_count,
                        (array_agg(tlanesu))[1] tlane_count,
                        array_agg(laneno) as laneno_array,
                        array_agg(flanecd) as add_array,
                        (cast( sum(1<< (tlanesu - laneno)) as bit(16)))::varchar as laneinfo ,  
                        (array_agg((case when cast(ypaint as integer) = 0 then 1
                               when cast(ypaint as integer) > 0 and cast(ypaint as integer) < 90 then 2
                               when cast(ypaint as integer) = 90 then 4
                               when cast(ypaint as integer) > 90 and cast(ypaint as integer) < 180 then 8
                               when cast(ypaint as integer) = 180 then 2048
                               when cast(ypaint as integer) = -180 then 16
                               when cast(ypaint as integer) > -180 and cast(ypaint as integer) < -90 then 32
                               when cast(ypaint as integer) = -90 then 64
                               when cast(ypaint as integer) > -90 and cast(ypaint as integer) < 0 then 128
                               else 0 end ))
                            
                         )[1] as arrowinfo,
                        (array_agg(b.lane_numl))[1] as lanenuml,
                        (array_agg(b.lane_numr))[1] as lanenumr             
                    from org_lane as a
                    left join
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

                    ) as b 
                    using(meshcode,tnodeno,snodeno,enodeno)
                    group by a.meshcode,a.tnodeno,a.snodeno,a.enodeno,a.ysnodeno,a.yenodeno
                    
                ) as aa
                
                left join org_road as c
                   on ( aa.snodeno =c.snodeno and aa.enodeno = c.enodeno
                       or aa.enodeno =c.snodeno and aa.snodeno = c.enodeno
                       )
                       and aa.meshcode = c.meshcode
                left join temp_link_mapping as tlk1
                on tlk1.meshcode = c.meshcode and tlk1.linkno = c.linkno
                       
                left join org_road as d
                   on ( aa.ysnodeno = d.snodeno and aa.yenodeno = d.enodeno
                        or aa.yenodeno =d.snodeno and aa.ysnodeno = d.enodeno
                       )
                       and aa.meshcode = d.meshcode
                       
                left join temp_link_mapping as tlk2
                on tlk2.meshcode = d.meshcode and tlk2.linkno = d.linkno

                left join temp_node_mapping as tnd
                on tnd.meshcode = aa.meshcode and tnd.nodeno = aa.tnodeno     
      
                left join
                (
                  select  bb.*,zenrin_findpasslink(bb.meshcode, bb.tnodeno,bb.ysnodeno,bb.passageno_array) as passlink,
                    zenrin_findpasslink_count(zenrin_findpasslink(bb.meshcode, bb.tnodeno,bb.ysnodeno,bb.passageno_array)) as passlink_cnt
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
                ) as p
              on aa.meshcode=p.meshcode and aa.tnodeno=p.tnodeno and  aa.snodeno= p.snodeno 
              and aa.enodeno=p.enodeno and aa.ysnodeno=p.ysnodeno and aa.yenodeno= p.yenodeno
          
          order by tnd.node_id, tlk1.link_id, tlk2.link_id, p.passlink, aa.tlane_count,
                   aa.laneinfo, aa.arrowinfo, aa.lanenuml, aa.lanenumr
                              
        )
    
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
                print 'error'
                continue
            if result[5][0]<>result[5][1]:
                print 'tnodeno error!'
                continue
            tnodeno=result[5][0]
            slaneinf_l=slaneinf.find('1')+1
            slaneinf_r=slaneinf.rfind('1')+1
            elaneinf_all=rjust(str(int(elaneinf_1)+int(elaneinf_2)),16,'0')
            
            if elaneinf_all.strip('0').find('0')<>-1:
                continue
            
            elaneinf_l = 17 - len(elaneinf_all.lstrip('0'))
            elaneinf_r = len(elaneinf_all.rstrip('0'))
            lanenum = elaneinf_r - elaneinf_l+1
            lanenum_l = slaneinf_l - elaneinf_l
            lanenum_r = elaneinf_r - slaneinf_r
            
            if lanenum_l<0 or lanenum_r<0:
                continue
                       
            slaneinfo_1 = elaneinf_1[elaneinf_l-1:elaneinf_r]
            slaneinfo_2 = elaneinf_2[elaneinf_l-1:elaneinf_r]
            
#            slaneinfo = slaneinf[elaneinf_l-1:elaneinf_r]
#            slaneinfo_1 = rjust(slaneinfo_1,16,'0')
#            slaneinfo_2 = rjust(slaneinfo_2,16,'0') 
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
            
#            laneinf_all = rjust(str(int(elaneinf)+int(slaneinf)),16,'0')
#            laneinf_l = 17 - len(laneinf_all.lstrip('0'))
#            laneinf_r = len(laneinf_all.rstrip('0'))
            
            slaneinfo = slaneinf[slaneinf_l-1:slaneinf_r]
#            elaneinfo = elaneinf[laneinf_l-1:laneinf_r]
        
          
            lanenum_l = 0
            lanenum_r = 0 
            arrow = 0    
            self.pg.execute(sqlcmd_insert%(tnodeno,meshcode,sroadno,eroadno,lanenum,slaneinfo,arrow,lanenum_l,lanenum_r))
            self.pg.commit2()
        
        self.CreateIndex2('temp_hwlane_tbl_meshcode_idx')
        self.CreateIndex2('temp_hwlane_tbl_inlinkid_idx')
        self.CreateIndex2('temp_hwlane_tbl_outlinkid_idx')
        
        

 
        
   
        
        
        
        
        
