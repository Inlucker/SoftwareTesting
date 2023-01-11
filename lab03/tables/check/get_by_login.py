from time import time

from tables.check.base import CheckBase
from tables.models import User


class CheckGetByLogin(CheckBase):
    name = 'get_by_login'
    graph_title = 'Get by login'

    def check_rows(self, rows):
        User.objects.all().delete()
        for i in range(rows):
            User.objects.create(id=i, login=f'testlogin{i}', password='testpas', role='testrole',
                                 moderator_id=None)
        start_time = time()
        for i in range(rows):
            User.objects.get(login=f'testlogin{i}')
        time_taken = time() - start_time
        return time_taken
