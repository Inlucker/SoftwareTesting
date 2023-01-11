import json
from abc import abstractmethod
from os import path

import matplotlib.pyplot as plt
from django.conf import settings


class CheckBase:
    name = ''
    graph_title = ''
    ROWS_CONST = [10, 100, 500]
    res = {10: 0, 100: 0, 500: 0}


    @abstractmethod
    def check_rows(self, row):
        pass

    def check_all(self, rows):
        print(f"Benchmarking {settings.DB_ENGINE} {self.name}")
        data = []
        print(f'Testing inset time {rows}')
        time_taken = self.check_rows(rows)
        print(f'completed in {time_taken}')
        data.append(dict(
            rows=rows,
            time=time_taken,
        ))
        return data

    def file_name(self, engine, ):
        return path.join(settings.BASE_DIR, f'reports/data/{engine}_{self.name}.txt')

    def save_file(self, data):
        file_name = self.file_name(settings.DB_ENGINE)
        with open(file_name, 'a+') as fs:
            print(data)
            fs.write(str(data[0]['rows']))
            fs.write(' ')
            fs.write(str(data[0]['time']))
            fs.write('\n')

    def run(self):
        for rows in self.ROWS_CONST:
            data = self.check_all(rows)
            self.save_file(data)

    def create_graph(self):
        index = []
        values = []
        for engine in settings.DB_ENGINES:
            self.res = {10: 0, 100: 0, 500: 0}
            file_name = self.file_name(engine, )
            if not path.isfile(file_name):
                continue
            with open(file_name, 'r') as fs:
                count = 0
                while True:
                    line = fs.readline()
                    line = line.split()
                    if not line:
                        break
                    self.res[int(line[0])] += float(line[1])
                    count += 1
            count /= len(self.ROWS_CONST)
            for i in self.ROWS_CONST:
                self.res[i] /= count
                values.append(self.res[i])
                index.append(engine)

        my_colors = ('#047495', '#fac205', '#003045', '#63b2e0', '#6933ff')
        fig, ax = plt.subplots(1, len(self.ROWS_CONST))

        fig.set_figwidth(15)
        fig.set_figheight(5)
        for i in range(len(self.ROWS_CONST)):
            indexg = index[i::len(self.ROWS_CONST)]
            valueg = values[i::len(self.ROWS_CONST)]
            ax[i].bar(indexg, valueg, color=my_colors)
            ax[i].set_ylabel('Time(s)')
            ax[i].set_xlabel('Database')
            ax[i].set_title(self.graph_title)
            ax[i].legend()

        file_name = path.join(settings.BASE_DIR, f'reports/graphs/{self.name}.png')
        plt.savefig(path.join(settings.BASE_DIR, f'reports/graphs/{self.name}.png'), format='png')
        print('file saved', file_name)
