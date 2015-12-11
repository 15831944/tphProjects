# coding:utf-8

import platform.TestCase

class CCheckIDDuplication(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(1) from
                    (
                        select meshcode,linkno from org_road group by meshcode,linkno having count(1)>1
                    ) a
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckLinkCircle(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(*) from org_road
            where st_equals(st_startpoint(the_geom_4326),st_endpoint(the_geom_4326))
                 '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0) 
    
class CCheckZLink(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateFunction_ByName('test_link_shape_turn_number')
        sqlcmd='''
            select count(*) from org_road
            where test_link_shape_turn_number(the_geom_4326)>0
               '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0) 
        
class CCheckFKYValid_jnctid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from org_road a
            left join org_node b
            on a.snodeno = b.nodeno and a.meshcode=b.meshcode
            left join org_node c
            on a.enodeno = c.nodeno and a.meshcode=c.meshcode    
            where b.meshcode is null or c.meshcode is null
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
    
class CCheckOverlay(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateIndex_ByName('org_road_meshcode_linkno_idx')
        self.pg.CreateIndex_ByName('org_road_snodeno_enodeno_idx')
        sqlcmd = """
            select count(*) from org_road a
            inner join org_road b
            on a.meshcode=b.meshcode and ((a.snodeno  = b.enodeno and a.enodeno = b.snodeno)
            or (a.snodeno = b.snodeno and a.enodeno = b.enodeno))
            and a.linkno <> b.linkno and st_equals(a.the_geom_4326,b.the_geom_4326);
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckCircle(platform.TestCase.CTestCase):
    def _do(self):
        self.pg.CreateIndex_ByName('org_road_meshcode_linkno_idx')
        self.pg.CreateIndex_ByName('org_road_snodeno_enodeno_idx')
        sqlcmd = """
            select count(*) from org_road a
            inner join org_road b
            on a.meshcode=b.meshcode and ((a.snodeno  = b.enodeno and a.enodeno = b.snodeno)
            or (a.snodeno = b.snodeno and a.enodeno = b.enodeno))
            and a.linkno <> b.linkno ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckOneway(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd='''
            select count(1) from org_road
            where oneway not in (0,1,2)
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckNetlevel(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd='''
            select count(1) from org_road
            where netlevel not in (0,1,2,3,4,5)
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckProv_exp(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
            select 
            (
                select count(*) from org_prov_exp where substr(meshcode,1,1)<>'Z' or length(meshcode)<>7
            )
            +
            (
                select count(*)-count(distinct(meshcode,linkno)) from org_prov_exp
            )
            +
            (
                select count(*) from org_prov_exp a 
                left join org_road b
                on substr(a.meshcode,2)=b.meshcode and a.linkno=b.linkno
                where b.meshcode is null
            )
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckGeomType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
            select count(*) from org_road where geometrytype(the_geom)<>'MULTILINESTRING'
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        if reg_count<>0 :
            return False
        sqlcmd='''
            select count(*) from org_road where st_numgeometries(the_geom)<>1
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckTunnel(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''
                select 
                (
                    select count(*) from org_tunnelname a left join org_road b
                     on a.meshcode = b.meshcode and a.roadno=b.linkno
                     where a.tcode='1' and substr(b.elcode,5,1)<>'2' or b.meshcode is null
                )
                 +
                (
                    select count(*) from org_underpass a left join org_road b
                     using(meshcode,linkno)
                     where substr(b.elcode,5,1)<>'2' or b.meshcode is null 
                )
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckRoadName(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''
                select
                (
                    select count(*)-count(distinct(meshcode,linkno)) from org_underpass
                )
                +
                (
                    select count(*)-count(distinct(meshcode,roadno)) from org_tunnelname
                )
                +
                (
                    select count(*)-count(distinct(meshcode,roadno)) from org_rname_bpmf
                )
                +
                (
                    select count(*)-count(distinct(meshcode,roadno)) from org_l5l6name
                )
                '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckRoadNumber(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd='''
                select count(1) from org_prov_exp
                where position ('省道' in routeno)=0
               '''
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckTimeCross(platform.TestCase.CTestCase):
    
    def _checktimecross(self,all_time):
        
        n=len(all_time)
        for i in range(n-1):
            for j in range(i+1,n):
                if not self._checktimecross_cond(all_time[i],all_time[j]):
                    return False
        return True
        
    def _checktimecross_cond(self,cond_1,cond_2):
                
        sdate1=int(cond_1[3])
        edate1=int(cond_1[4])
        sdate2=int(cond_2[3])
        edate2=int(cond_2[4])
        
        if sdate1>=sdate2:
            sdate_cross=sdate1
        else:
            sdate_cross=sdate2
        
        if edate1<=edate2:
            edate_cross=edate1
        else:
            edate_cross=edate2
          
        day_of_week_1=cond_1[0]
        day_of_week_2=cond_2[0]
          
        if day_of_week_1[0]=='1' and day_of_week_2[0]=='1':
            day_of_week_cross=128
        elif day_of_week_1[0]=='1':
            day_of_week_cross=int(day_of_week_2)
        elif day_of_week_2[0]=='1':
            day_of_week_cross=int(day_of_week_1)
        else:
            day_of_week_cross=int(day_of_week_1) & int(day_of_week_2)
            
        shour1=int(cond_1[1])
        ehour1=int(cond_1[2])
        shour2=int(cond_2[1])
        ehour2=int(cond_2[2])
        
        if shour1>=shour2:
            shour_cross=shour1
        else:
            shour_cross=shour2
            
        if ehour1<=ehour2:
            ehour_cross=ehour1
        else:
            ehour_cross=ehour2
        
        if sdate1>edate1 or sdate2>edate2 or shour1>ehour1 or shour2>ehour2:
            return False
        
        if sdate_cross<edate_cross or day_of_week_cross==0 or shour_cross<ehour_cross:
            return True
        else:
            return False
    def _do(self):
        
        self.pg.CreateTable_ByName('temp_link_dummy_walked')
        sqlcmd = ''' 
                select meshcode,linkno from org_road where --"meshcode"='342002'  and linkno >= 22580 and linkno <=22583
                substr(elcode,4,1)='8'
                '''      
        
        self.pg.execute2(sqlcmd)
        alldummylink=self.pg.fetchall2()
        #print a
        
        sqlcmd_2='''
                select count(1) from temp_link_dummy_walked
                where meshcode='%s' and linkno=%d
                    '''
        sqlcmd_insert='''
                insert into temp_link_dummy_walked
                (meshcode,linkno)
                values('%s',%d)
                        '''
        for i in alldummylink:
            self.pg.execute2(sqlcmd_2%(i[0],i[1]))
            cnt=self.pg.fetchall2()[0][0]
            if cnt==0:
                dummylink_arr=[i]
                self.pg.execute2(sqlcmd_insert%(i[0],i[1]))
                self.pg.commit2()
                while True:
                    cnt1=0
                    for j in dummylink_arr:
                        
                        sqlcmd_3='''
                        select b.meshcode,b.linkno from org_road a 
                        join org_road b
                        on st_intersects(a.the_geom,b.the_geom) and (substr(b.elcode,4,1)='8' or substr(b.elcode,4,1)='6')
                        left join temp_link_dummy_walked c
                        on b.meshcode=c.meshcode and b.linkno=c.linkno
                        where a.meshcode='%s' and a.linkno=%d and c.linkno is null
                                '''
                        self.pg.execute2(sqlcmd_3%(j[0],j[1]))
                        results=self.pg.fetchall2()
                        cnt1+=len(results)
                        
                        for result in results:
                            dummylink_arr.append(result)
                            self.pg.execute2(sqlcmd_insert%(result[0],result[1]))
                            self.pg.commit2()
                    if cnt1==0:
                        break
                print 'all_link:',dummylink_arr

                self.pg.CreateTable_ByName('temp_dummy_link')
                sqlcmd_insertdummy='''
                        insert into temp_dummy_link
                        (meshcode,linkno,snodeno,enodeno,oneway,elcode,the_geom)
                        select meshcode,linkno,snodeno,enodeno,oneway,elcode,the_geom
                        from org_road
                        where meshcode='%s' and linkno=%d
                                    '''
                for i in dummylink_arr:
                    self.pg.execute2(sqlcmd_insertdummy%(i[0],i[1]))
                    self.pg.commit2()
                #exit()
                sqlcmd_alllinknear='''
                        insert into temp_dummy_link
                        (meshcode,linkno,snodeno,enodeno,oneway,elcode,the_geom)
                        select distinct a.meshcode,a.linkno,a.snodeno,a.enodeno,a.oneway,a.elcode,a.the_geom from org_road a
                        join 
                        temp_dummy_link b
                        on st_intersects(a.the_geom,b.the_geom)
                        left join temp_dummy_link c
                        on a.meshcode=c.meshcode and a.linkno=c.linkno
                        where c.linkno is null
                        '''
                self.pg.execute2(sqlcmd_alllinknear)
                self.pg.commit2()
                    #continue
                sqlcmd_reg='''
                        select count(1) from "org_not-in" a 
                        join temp_dummy_link b on a.meshcode=b.meshcode and a.fromlinkno=b.linkno
                        join temp_dummy_link c on a.meshcode=c.meshcode and a.tolinkno=c.linkno
                        where day<>'100000000' or shour<>'0000' or ehour<>'2400' or sdate<>'0101' or edate<>'1231'
                            '''
                self.pg.execute2(sqlcmd_reg)
                count=self.pg.fetchall2()[0][0]
                if count>0:
                    sqlcmd_1='''
                        select distinct day,shour,ehour,sdate,edate from "org_not-in" a
                        join temp_dummy_link b on a.meshcode=b.meshcode and a.fromlinkno=b.linkno
                        join temp_dummy_link c on a.meshcode=c.meshcode and a.tolinkno=c.linkno
                        where day<>'100000000' or shour<>'0000' or ehour<>'2400' or sdate<>'0101' or edate<>'1231'
                                '''
                    self.pg.execute2(sqlcmd_1)
                    all_time=self.pg.fetchall2()
                    if not self._checktimecross(all_time):
                        return False
        
        return True