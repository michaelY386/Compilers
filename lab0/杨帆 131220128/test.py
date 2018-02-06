# coding = utf-8
import urllib.request
import re

def main():

	my_cookie = 'SINAGLOBAL=7801776186097.413.1456739293704; wvr=6; YF-Page-G0=046bedba5b296357210631460a5bf1d2; _s_tentry=-; Apache=31493308488.27958.1456739883431; ULV=1456739883551:2:2:2:31493308488.27958.1456739883431:1456739293714; YF-V5-G0=32eb5467e9bfc8b60c2d771056535ac5; YF-Ugrow-G0=57484c7c1ded49566c905773d5d00f82; SUS=SID-3477701200-1456739891-GZ-mc2zr-3c48a85f2452ad3b20b2adbd184e539d; SUE=es%3D12bf963413eaba32d8a9740915afaa62%26ev%3Dv1%26es2%3D098f3364a647d1d4cfaa91e247479599%26rs0%3DC53ai1IvvpBPBUSzc60%252F9iiED2edCw0wzakElvHje40P08mDhDG89qJ%252FcEwEPPaWklQkn8oxtpxMwKXTxVDWSZCZX29A1jQAE1Dif1SHL60O6dEib5uCOATWj3vrjtO957TQW90FCR5V%252B8oRu75ywu2s5orYOtZO2YXRFyi7gxs%253D%26rv%3D0; SUP=cv%3D1%26bt%3D1456739891%26et%3D1456826291%26d%3Dc909%26i%3D539d%26us%3D1%26vf%3D0%26vt%3D0%26ac%3D2%26st%3D0%26uid%3D3477701200%26name%3D18001591168%26nick%3D%25E7%2594%25A8%25E6%2588%25B73477701200%26fmp%3D%26lcp%3D2015-02-07%252015%253A25%253A40; SUB=_2A2570GZjDeRxGeVK7FUW8C_OyzyIHXVYpNCrrDV8PUNbu9BeLVDukW9LHeuDha5UAKzQAWqWdmytHtRPYqOLOg..; SUBP=0033WrSXqPxfM725Ws9jqgMF55529P9D9WFSed5F-2-W25OhVFIiCM-75JpX5Kzt; SUHB=08JLeSmgc_WiwK; ALF=1488275890; SSOLoginState=1456739891'
	f = open(r'C:/Users/App/result.txt','w')
	x = 0
	#记录提取信息用户数 直到x=100
	for i in range(1,5):
		headers = {'cookie': my_cookie}
		#global follow_url
		follow_url = 'http://weibo.com'
	#follow_url = 'http://weibo.com/3477701200/follow?from=page_100505&wvr=6&mod=headfollow#place'
	#好友列表一页只显示30人
		if i == 1:
			follow_url = 'http://weibo.com/p/1005053477701200/myfollow?t=1&cfs=&Pl_Official_RelationMyfollow__111_page=1#Pl_Official_RelationMyfollow__111'
		elif i == 2: 
			follow_url = 'http://weibo.com/p/1005053477701200/myfollow?t=1&cfs=&Pl_Official_RelationMyfollow__111_page=2#Pl_Official_RelationMyfollow__111'
		elif i == 3: 
			follow_url = 'http://weibo.com/p/1005053477701200/myfollow?t=1&cfs=&Pl_Official_RelationMyfollow__111_page=3#Pl_Official_RelationMyfollow__111'
		elif i == 4: 
			follow_url = 'http://weibo.com/p/1005053477701200/myfollow?t=1&cfs=&Pl_Official_RelationMyfollow__111_page=4#Pl_Official_RelationMyfollow__111'
		elif i == 5: 
			follow_url = 'http://weibo.com/p/1005053477701200/myfollow?t=1&cfs=&Pl_Official_RelationMyfollow__111_page=5#Pl_Official_RelationMyfollow__111'
		res = urllib.request.Request(follow_url, headers = headers)
		r = urllib.request.urlopen(res) 
		html = r.read()

		#python 3.* 中需要转码 才可以被re模块中方法识别
		codec = r.info().get_param('charset', 'utf8')
		html = html.decode(codec)
	
		pal_pattern = re.compile(r'uid=\d+&nick=')
		pallist = pal_pattern.findall(html)

		global flag
		flag = 1
		#loop
		for pal in pallist:
			#提取个人主页 pal_html
			uid = pal[4:len(pal)-6]
			pal_url = 'http://weibo.com/u/'+uid+'?from=myfollow_all'
			pal_res = urllib.request.Request(pal_url, headers = headers)
			pal_r = urllib.request.urlopen(pal_res)
			pal_html = pal_r.read()
		
			#f.write(uid)
			codec = pal_r.info().get_param('charset', 'utf8')
			pal_html = pal_html.decode(codec)

			#提取个人详细信息的页面 detail_html
			personal_pat = re.compile(r'\$CONFIG\[\'page_id\'\]=\'\d+\'')
			match = personal_pat.search(pal_html)

			if match:
				flag = 1 #表示用户是否“信息合法”
				page_id = match.group()
				page_id = page_id[20:len(page_id)-1]
				detail_url = 'http://weibo.com/p/'+page_id+'/info?mod=pedit_more'
				detail_res = urllib.request.Request(detail_url, headers = headers)
				detail_r = urllib.request.urlopen(detail_res)
				detail_html = detail_r.read()

				#匹配每个人不同的属性
				codec = detail_r.info().get_param('charset', 'utf8')
				detail_html = detail_html.decode(codec)

				#$CONFIG['onick']='李开复'   匹配昵称信息
				nick_pat = re.compile(r'\$CONFIG\[\'onick\'\]=\'.+\'')
				match1 = nick_pat.search(detail_html)
				if match1:
					nickname = match1.group()
					nickname = nickname[18:len(nickname)-1]
					if nickname == '喝了蜜的大狮子_micheal': 
						#flag = 0
						continue                             					 #无法爬取昵称的部分用户，在显示昵称时会显示成我的微博id，这类用户我选择跳过处理
					if flag: 
						#打印昵称
						f.write(nickname+', ')
				#打印用户id
				print(page_id)
				f.write(page_id+', ') 
				
				#基本信息
				#匹配所在地信息
				local_pat = re.compile(r'\<span class=\\\"pt_title S_txt2\\\"\>所在地：.+?clearfix\\\"')
				match1 = local_pat.search(detail_html)
				if match1:
					place = match1.group()
					place = place[70:len(place)-89]
					#打印所在地
					f.write(place+', ')
				else: 
					f.write('null, ')
			
				#匹配性别信息
				gender_pat = re.compile(r'\<span class=\\\"pt_title S_txt2\\\"\>性别：.+?\<\\\/span\>\<\\\/li\>')
				match2 = gender_pat.search(detail_html)
				if match2:
					gender = match2.group()
					gender = gender[69:len(gender)-14]
					f.write(gender+', ')
				else: f.write('null, ')
				
				#匹配感情状况信息
				relation_pat = re.compile(r'感情状况：.+?\<\\\/li\>')
				match3 = relation_pat.search(detail_html)
				if match3:
					relation = match3.group()
					relation = relation[107:len(relation)-72]
					f.write(relation+', ')
				else: f.write('null, ')
				
				#匹配生日信息
				birth_pat = re.compile(r'\<span class=\\\"pt_title S_txt2\\\"\>生日：.+?\<\\\/span\>\<\\\/li\>')
				match4 = birth_pat.search(detail_html)
				if match4:
					birth = match4.group()
					birth = birth[69:len(birth)-14]
					f.write(birth+', ')
				else: f.write('null, ')

				#匹配博客信息
				blog_pat = re.compile(r'博客：(.+?)a href=.+?from')
				match5 = blog_pat.search(detail_html)
				if match5:
					blog = match5.group()
					blog = blog[57:len(blog)-5]
					strinfo = re.compile('\\\/')
					blog = strinfo.sub('/', blog)
					f.write(blog+', ')
				else: f.write('null, ')
				
				#匹配个性域名信息
				zone_pat = re.compile(r'个性域名：(.+?)a href=.+?from')
				match6 = zone_pat.search(detail_html)
				if match6:
					zone = match6.group()
					zone = zone[123:len(zone)-5]
					strinfo = re.compile('\\\/')
					zone = strinfo.sub('/', zone)
					f.write(zone+', ')
				else: f.write('null, ')
				'''
				#<span class=\"pt_title S_txt2\">简介：<\/span>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t<span class=\"pt_detail\">创新工场CEO，媒体联系：press@chuangxin.com<\/span>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t<\/li>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t<li class=\"li_1 clearfix\"> 匹配简介信息
				intro_pat = re.compile(r'简介：(.+?)\<\\\/li\>')
				match1 = intro_pat.search(detail_html)
				if match1:
					intro = match1.group()
					intro = intro[77:len(intro)-52]
					f.write(intro+', ')
				else: f.write('null, ')
				'''
				#注册时间：匹配注册时间
				reg_pat = re.compile(r'注册时间：(.+?)\<\\\/li\>')
				match7 = reg_pat.search(detail_html)
				if match7:
					reg = match7.group()
					reg = reg[103:len(reg)-70]
					f.write(reg+', ')
				else: f.write('null, ')

				#工作信息
				#公司 时间 地区 职位
				work_pat = re.compile(r'公司：.+?模块')
				match8 = work_pat.search(detail_html)
				if match8:
					work = match8.group()
					#公司
					company_pat = re.compile(r'blank.+?\<\\\/a\>')
					match9 = company_pat.search(work)
					if match9:
						company = match9.group()
						company = company[8:len(company)-5]
						f.write(company+', ')
					else: 
						f.write('null, ')
					#时间
					time_pat = re.compile(r'\\t \(.+?\<br\\\/\>')
					match10 = time_pat.search(work)
					if match10:
						time = match10.group()
						time = time[3:len(time)-52]
						f.write(time+', ')
					else: 
						f.write('null, ')
					#地区
					area_pat = re.compile(r'地区.+?\<br\\\/\>')
					match11 = area_pat.search(work)
					if match11:
						area = match11.group()
						area = area[3:len(area)-6]
						f.write(area+', ')
					else: 
						f.write('null, ')
					#职位
					pos_pat = re.compile(r'职位.+?\\t')
					match12 = pos_pat.search(work)
					if match12:
						pos = match12.group()
						pos = pos[3:len(pos)-2]
						f.write(pos+', ')
					else: 
						f.write('null, ')
				else: 
					f.write('null, ')
					f.write('null, ')
					f.write('null, ')
					f.write('null, ')

				#教育信息
				#大学 年份 系别
				uni_pat = re.compile(r'大学：.+?模块')
				match13 = uni_pat.search(detail_html)
				if match13:
					uni = match13.group()
					#大学
					uniname_pat = re.compile(r'infedu.+?\<\\\/a\>')
					match14 = uniname_pat.search(uni)
					if match14:
						uniname = match14.group()
						uniname = uniname[9:len(uniname)-5]
						f.write(uniname+', ')
					else: 
						f.write('null, ')
					#系别
					dep_pat = re.compile(r'\\t   .+?\\t')
					match15 = dep_pat.search(uni)
					if match15:
						dep = match15.group()
						dep = dep[5:len(dep)-2]
						f.write(dep+', ')
					else: 
						f.write('null, ')
				else: 
					f.write('null, ')
					f.write('null, ')
				#高中
				sen_pat1 = re.compile(r'高中：.+?\<\\\/a\>')
				match16 = sen_pat1.search(detail_html)
				if match16:
					sen1 = match16.group()
					sen_pat = re.compile(r'infedu.+?\<\\\/a\>')
					match18 = sen_pat.search(sen1)
					if match18:
						sen = match18.group()
						sen = sen[9:len(sen)-5]
						f.write(sen+', ')
				else: 
					f.write('null, ')

				#初中
				jun_pat1 = re.compile(r'初中：.+?\<\\\/a\>')
				match17 = jun_pat1.search(detail_html)
				if match17:
					jun1 = match17.group()
					jun_pat = re.compile(r'infedu.+?\<\\\/a\>')
					match19 = jun_pat.search(jun1)
					if match19:
						jun = match19.group()
						jun = jun[9:len(jun)-5]
						f.write(jun+', ')
				else: 
					f.write('null, ')

				#联系信息
				#邮箱 QQ
				mail_pat = re.compile(r'邮箱：.+?\<\\\/li\>')
				match20 = mail_pat.search(detail_html)
				if match20:
					mail = match20.group()
					mail = mail[37:len(mail)-14]
					f.write(mail+', ')
				else: 
					f.write('null, ')

				qq_pat = re.compile(r'QQ：.+?\<\\\/li\>')
				match21 = qq_pat.search(detail_html)
				if match21:
					qq = match21.group()
					qq = qq[63:len(qq)-38]
					f.write(qq)
				else: 
					f.write('null')
				f.write('\n')

			if x == 100:
				break
		#end of loop
	f.close()


if __name__=='__main__':
	main()
