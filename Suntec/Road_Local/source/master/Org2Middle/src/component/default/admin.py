'''
Created on 2012-2-23

@author: sunyifeng
'''

import component.component_base

class comp_admin(component.component_base.comp_base):
    
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Admin') 
            
    def _Do(self):
        self.CreateTable2('mid_admin_zone')
        self.CreateTable2('mid_admin_summer_time')
        
    def make_geom_for_order(self):       
        self.log.info('Now make admin order geom...')
        #update geometry ad_order = 2 and ad_code is not null
        sqlcmd = """
                UPDATE mid_admin_zone AS a SET the_geom = b.the_geom
                FROM
                (
                    SELECT a.ad_code,
                           st_multi (st_union (b.the_geom)) AS the_geom
                    FROM mid_admin_zone AS a
                    INNER JOIN mid_admin_zone AS b
                    ON a.ad_order = 2 AND a.the_geom IS NULL AND b.ad_order = 8
                       AND b.order2_id = a.ad_code
                    GROUP by a.ad_code
                ) as b
                WHERE a.ad_code = b.ad_code
                """

        self.log.info('Now it is updating mid_admin_zone ad_order = 2...')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('updating mid_admin_zone ad_order = 2 succeeded')

        #update geometry if ad_order = 1 and ad_code is not null
        sqlcmd = """
                UPDATE mid_admin_zone AS a SET the_geom = b.the_geom
                FROM
                (
                    SELECT a.ad_code,
                           st_multi (st_union (b.the_geom)) AS the_geom
                    FROM mid_admin_zone AS a
                    INNER JOIN mid_admin_zone AS b
                    ON a.ad_order = 1 AND a.the_geom IS NULL AND b.ad_order = 8
                       AND b.order1_id = a.ad_code
                    GROUP by a.ad_code
                ) as b
                WHERE a.ad_code = b.ad_code
                """

        self.log.info('Now it is updating mid_admin_zone ad_order = 1...')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('updating mid_admin_zone ad_order = 1 succeeded')

        #update geometry if ad_order = 0 and ad_code is not null
        sqlcmd = """
                UPDATE mid_admin_zone AS a SET the_geom = b.the_geom
                FROM
                (
                    SELECT a.ad_code,
                           st_multi (st_union (b.the_geom)) AS the_geom
                    FROM mid_admin_zone AS a
                    INNER JOIN mid_admin_zone AS b
                    ON a.ad_order = 0 AND a.the_geom IS NULL AND b.ad_order = 8
                       AND b.order0_id = a.ad_code
                    GROUP by a.ad_code
                ) as b
                WHERE a.ad_code = b.ad_code
                """

        self.log.info('Now it is updating mid_admin_zone ad_order = 0...')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def do_admin_time_order(self):
        
        # update admin's time_zone & summer_time
        sqlcmd = """
            update mid_admin_zone as a 
            set [time_attr] = b.[time_attr]
            from
            (
                select ad_code, [time_attr]
                from mid_admin_zone
                where ad_order = 0 and [time_attr] is not null
            )as b
            where a.ad_order = 1 and a.[time_attr] is null and a.order0_id = b.ad_code;
            ;
            
            update mid_admin_zone as a 
            set [time_attr] = b.[time_attr]
            from
            (
                select ad_code, [time_attr]
                from mid_admin_zone
                where ad_order = 1 and [time_attr] is not null
            )as b
            where a.ad_order = 2 and a.[time_attr] is null and a.order1_id = b.ad_code;
            ;
            
            update mid_admin_zone as a 
            set [time_attr] = b.[time_attr]
            from
            (
                select ad_code, [time_attr]
                from mid_admin_zone
                where ad_order in (1,2) and [time_attr] is not null
            )as b
            where  a.ad_order = 8 and a.[time_attr] is null 
                   and 
                   (
                       (a.order2_id = b.ad_code)
                       or
                       (a.order2_id is null and a.order1_id = b.ad_code)
                   )
            ;
        """
        for time_attr in ['time_zone', 'summer_time_id']:
            self.pg.execute2(sqlcmd.replace('[time_attr]', time_attr))
            self.pg.commit2()
            
    def CreateOrder1(self):
        self.log.info('create order1...')
        sqlcmd = """
                select distinct order0_id
                from mid_admin_zone
                where order1_id is null and ad_order in (2,8);
                """
        self.pg.execute2(sqlcmd)
        row_list = self.pg.fetchall2()
        
        for row in row_list:
            sqlcmd = """
                    select count(*)
                    from mid_admin_zone
                    where order0_id =%d and ad_order = 2;
                    """%row[0]
            self.pg.execute2(sqlcmd)
            num = self.pg.fetchone2()[0]
            if num == 0:
                #create order1
                sqlcmd = """
                        insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, 
                                                   time_zone, summer_time_id, the_geom)
                        select id as ad_code, 
                               1 as ad_order, 
                               order0_id, 
                               id as order1_id,
                               time_zone,
                               summer_time_id,
                               the_geom      
                        from mid_admin_zone as a
                        left join 
                        (
                            select max(ad_code) + 1 as id
                            from mid_admin_zone
                            where order0_id = %d
                        )b
                        on TRUE
                        where order0_id = %d and ad_order = 0;
                        """%(row[0], row[0])
                self.pg.execute2(sqlcmd)
                self.pg.commit2()
                
                #update order8
                sqlcmd = """
                        UPDATE mid_admin_zone as a
                        set order1_id = b.ad_code
                        from(
                            select order0_id, ad_code
                            from mid_admin_zone
                            where order0_id = %d and ad_order = 8
                        )b
                        where a.ad_order = 8 and a.order0_id = b.order0_id;
                        """%row[0]
                self.pg.execute2(sqlcmd)
                self.pg.commit2()
            else:
                #order2 -> order1
                sqlcmd = """
                        UPDATE mid_admin_zone
                        set ad_order = 1, order1_id = order2_id, order2_id = null
                        where ad_order = 2 and order0_id = %d;
                        """%row[0]
                self.pg.execute2(sqlcmd)
                self.pg.commit2()
                
                #order8
                sqlcmd = """
                        UPDATE mid_admin_zone
                        set order1_id = order2_id, order2_id = null
                        where ad_order = 8 and order0_id = %d;
                        """%row[0]
                self.pg.execute2(sqlcmd)
                self.pg.commit2()            
