#!/usr/bin/env python
# encoding: utf-8

import sys
import xlsxwriter

workbook = xlsxwriter.Workbook('result.xlsx')
worksheet = workbook.add_worksheet()

row = input('請輸入寫入row,離開請按\"-1\" : ')
col = 0

while True:
    if row >= 0:

        is_draw_data = False
        is_average_num = False
        line_init = int()
        line_end = int()
        line_list = list()

        for string in open('round_put.txt'):
            if "node" in string:
                worksheet.write(row, col, string[:-1])
            elif 'average round' in string:
                is_draw_data = True
                line_init = row + 1
                worksheet.write(row, col, string[:-1])
            elif 'average num' in string:
                is_draw_data = False
                is_average_num = True
                line_end = row - 1
                worksheet.write(row, col, string[:-1])
            elif is_draw_data:
                line_list.append(float(string))
            elif is_average_num:
                worksheet.write(row, col , float(string))
            else:
                str_tmp = string.split()
                worksheet.write(row, col, str_tmp[0])
                worksheet.write(row, col + 1, str_tmp[1])
                worksheet.write(row, col + 2, str_tmp[2])

            row += 1

        worksheet.write_column('A'+str(line_init), line_list)
        chart = workbook.add_chart({'type': 'line'})

        chart.add_series({
            'values': 'Sheet1!$A$' + str(line_init) + ':$A$' + str(line_end),
            'marker': {'type': 'diamond'}
        })
        worksheet.insert_chart('C'+str(line_init), chart)

        print "end in : " + str(row)
        row = input('請輸入寫入行號,離開請按\"-1\" : ')
    elif row == -1:
        workbook.close()
        break

