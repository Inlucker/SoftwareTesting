from time import time

from tables.check.base import CheckBase
from tables.models import User


class CheckUpdate(CheckBase):
    name = 'update'
    graph_title = 'Update'

    def check_rows(self, rows):
        User.objects.all().delete()
        m = User.objects.create(id=0, login='testname', password='testpas', role='testrole',
                                 moderator_id=None)
        start_time = time()
        for i in range(rows):
            m.login = f'name{i}'
            m.save()
        time_taken = time() - start_time
        return time_taken
