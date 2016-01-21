# -*- coding: UTF8 -*-
'''
Created on 2014-12-18

@author: hcz
'''
import platform.TestCase


# =============================================================================
# CCheckICNo
# =============================================================================
class CCheckICNo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_service_info
          where ic_no not in (
            SELECT ic_no
              FROM rdb_highway_ic_info
          );
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckServerFlag -- ic_no唯一
# =============================================================================


class CCheckServerFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM rdb_highway_service_info as a
          LEFT JOIN rdb_highway_ic_info as b
          ON a.ic_no = b.ic_no
          where b.servise_flag <> 1 or
                b.servise_flag is null;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckUnique -- ic_no唯一
# =============================================================================
class CCheckUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        SELECT count(*)
          FROM (
            SELECT ic_no
              FROM rdb_highway_service_info
              group by ic_no
              having count(*) > 1
          ) AS a;
        """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row and row[0] == 0:
            return True
        return False


# =============================================================================
# CCheckServiceCategory --服务情报在rdb_highway_service_category_mapping总能有对应配置
# =============================================================================
HWY_OASIS = 'Hwy oasis'
DOG_RUN = 'Dog run'
PUBLIC_TEL = 'Public telephone'
VENDING_MACHINE = 'Vending machine'
HANDICAPPED_TEL = 'Handicapped telephone'
HANDICAPPED_TOILET = 'Handicapped toilet'
INFORMATION = 'Info'
SNACK_CORNER = 'Snack corner'
NURSING_ROOM = 'Nursing room'
LAUNDERETTE = 'Launderette'
COIN_SHOWER = 'Coin shower'
TOILET = 'Toilet'
REST_AREA = 'Rest area'
SHOPPING_CORNER = 'Shopping corner'
FAX_SERVICE = 'Fax service'
POST_BOX = 'Post box'
HWY_INFOR_TERMINAL = 'Hwy infor terminal'
ATM = 'ATM'
HWY_HOT_SPRINGS = 'Hwy hot springs'
NAP_REST_AREA = 'Nap rest area'
RESTAURANT = 'Restaurant'
GAS_STATION = 'Gas station'


class CCheckServiceCategory(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select a.service, b.service as configed_service, category_id
        from (
            select distinct 'Dog run' as service
            from rdb_highway_service_info
            where dog_run = 1
            union
            select distinct 'Hwy oasis' as service
            from rdb_highway_service_info
            where hwy_oasis = 1
            union
            select distinct 'Public telephone' as service
            from rdb_highway_service_info
            where public_telephone = 1
            union
            select distinct 'Vending machine' as service
            from rdb_highway_service_info
            where vending_machine = 1
            union
            select distinct 'Handicapped telephone' as service
            from rdb_highway_service_info
            where handicapped_telephone = 1
            union
            select distinct 'Handicapped toilet' as service
            from rdb_highway_service_info
            where handicapped_toilet = 1
            union
            select distinct 'Info' as service
            from rdb_highway_service_info
            where information = 1
            union
            select distinct 'Snack corner' as service
            from rdb_highway_service_info
            where snack_corner = 1
            union
            select 'Nursing room' as service
            from rdb_highway_service_info
            where nursing_room = 1
            union
            select distinct 'Launderette' as service
            from rdb_highway_service_info
            where launderette = 1
            union
            select distinct 'Coin shower' as service
            from rdb_highway_service_info
            where coin_shower = 1
            union
            select distinct 'Toilet' as service
            from rdb_highway_service_info
            where toilet = 1
            union
            select distinct 'Rest area' as service
            from rdb_highway_service_info
            where rest_area = 1
            union
            select distinct 'Shopping corner' as service
            from rdb_highway_service_info
            where shopping_corner = 1
            union
            select distinct 'Fax service' as service
            from rdb_highway_service_info
            where fax_service = 1
            union
            select distinct 'Post box' as service
            from rdb_highway_service_info
            where postbox = 1
            union
            select distinct 'Hwy infor terminal' as service
            from rdb_highway_service_info
            where hwy_infor_terminal = 1
            union
            select distinct 'Nap rest area' as service
            from rdb_highway_service_info
            where nap_rest_area = 1
            union
            select distinct 'Restaurant' as service
            from rdb_highway_service_info
            where restaurant = 1
            union
            select distinct 'Gas station' as service
            from rdb_highway_service_info
            where gas_station = 1
        )as a
        left join rdb_highway_service_category_mapping as b
        on a.service = b.service
        """
        for row in self.pg.get_batch_data(sqlcmd):
            service, configed_service, category_id = row
            if not category_id:
                print('service not configed: %s' % service)
                return False
        return True
