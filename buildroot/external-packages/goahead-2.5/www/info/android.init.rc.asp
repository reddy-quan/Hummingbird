<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<!-- TemplateBeginEditable name="doctitle" -->
<title>无标题文档</title>
<!-- TemplateEndEditable -->
<!-- TemplateBeginEditable name="head" -->
<!-- TemplateEndEditable -->
<style type="text/css">
<!--
body {
	font: 100% Verdana, Arial, Helvetica, sans-serif;
	background: #666666;
	margin: 0; /* 最好将 body 元素的边距和填充设置为 0 以覆盖不同的浏览器默认值 */
	padding: 0;
	text-align: center; /* 在 IE 5* 浏览器中，这会将容器居中。文本随后将在 #container 选择器中设置为默认左对齐 */
	color: #000000;
}
.oneColFixCtrHdr #container {
	width: 780px;  /* 使用比最大宽度 (800px) 小 20px 的宽度可显示浏览器界面元素，并避免出现水平滚动条 */
	background: #FFFFFF;
	margin: 0 auto; /* 自动边距（与宽度一起）会将页面居中 */
	border: 1px solid #000000;
	text-align: left; /* 这将覆盖 body 元素上的“text-align: center”。 */
}
.oneColFixCtrHdr #header {
	background: #DDDDDD; 
	padding: 0 10px 0 20px;  /* 此填充会将出现在它后面的 div 中的元素左对齐。如果 #header 中使用的是图像（而不是文本），您最好删除填充。 */
}
.oneColFixCtrHdr #header h1 {
	margin: 0; /* 将 #header div 中最后一个元素的边距设置为零将避免边距重叠（即 div 之间出现的无法解释的空白）。如果 div 周围有边框，则不必将边距设置为零，因为边框也会避免边距重叠 */
	padding: 10px 0; /* 使用填充而不使用边距将可以使元素远离 div 的边缘 */
}
.oneColFixCtrHdr #mainContent {
	padding: 0 20px; /* 请记住，填充是 div 方块内部的空间，边距则是 div 方块外部的空间 */
	background: #FFFFFF;
}
.oneColFixCtrHdr #footer {
	padding: 0 10px; /* 此填充会将它上面 div 中的所有元素左对齐。 */
	background:#DDDDDD;
}
.oneColFixCtrHdr #footer p {
	margin: 0; /* 将脚注中第一个元素的边距设置为零将避免出现可能的边距重叠（即 div 之间出现的空白）*/
	padding: 10px 0; /* 就像边距会产生空白一样，此元素上的填充也将产生空白，但不会出现边距重叠问题 */
}
-->
</style></head>

<body class="oneColFixCtrHdr">

<div id="container">
  <div id="header">
<h1>Android init.rc解析</h1>
  <!-- end #header --></div>
  <div id="mainContent">
<div id="blog_article">
  <h4><span lang="EN-US" xml:lang="EN-US">Android init.rc (Android init   language)</span></h4>
  <p><span lang="EN-US" xml:lang="EN-US">Android</span>初始化语言由四大类声明组成<span lang="EN-US" xml:lang="EN-US">:</span>行为类<span lang="EN-US" xml:lang="EN-US">(Actions),</span>命令类<span lang="EN-US" xml:lang="EN-US">(Commands)</span>，服务类<span lang="EN-US" xml:lang="EN-US">(Services),</span>选项类<span lang="EN-US" xml:lang="EN-US">(Options).</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> * </span>初始化语言以行为单位，由以空格间隔的语言符号组成。<span lang="EN-US" xml:lang="EN-US">C</span>风格的反斜杠转义符可以用来插入空白到语言符号。双引号也可以用来防止文本被空格分成多个语言符号。当反斜杠在行末时，作为折行符。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> * </span>以<span lang="EN-US" xml:lang="EN-US">#</span>开始<span lang="EN-US" xml:lang="EN-US">(</span>前面允许有空格<span lang="EN-US" xml:lang="EN-US">)</span>的行为注释行。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> * Actions</span>和<span lang="EN-US" xml:lang="EN-US">Services</span>隐含声明一个新的段落。所有该段落下<span lang="EN-US" xml:lang="EN-US">Commands</span>或<span lang="EN-US" xml:lang="EN-US">Options</span>的声明属于该段落。第一段落前的<span lang="EN-US" xml:lang="EN-US">Commands</span>或<span lang="EN-US" xml:lang="EN-US">Options</span>被忽略。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> * Actions</span>和<span lang="EN-US" xml:lang="EN-US">Services</span>拥有独一无二的命名。在它们之后声明相同命名的类将被当作错误并忽略。</p>
  <p><strong><span lang="EN-US" xml:lang="EN-US">Actions</span></strong></p>
  <p><span lang="EN-US" xml:lang="EN-US">-------</span></p>
  <p><span lang="EN-US" xml:lang="EN-US">Actions</span>是一系列命令的命名。<span lang="EN-US" xml:lang="EN-US">Actions</span>拥有一个触发器<span lang="EN-US" xml:lang="EN-US">(trigger)</span>用来决定<span lang="EN-US" xml:lang="EN-US">action</span>何时执行。当一个<span lang="EN-US" xml:lang="EN-US">action</span>在符合触发条件被执行时，如果它还没被加入到待执行队列中的话，则加入到队列最后。</p>
  <p>队列中的<span lang="EN-US" xml:lang="EN-US">action</span>依次执行，<span lang="EN-US" xml:lang="EN-US">action</span>中的命令也依次执行。<span lang="EN-US" xml:lang="EN-US">Init</span>在执行命令的中间处理其它活动<span lang="EN-US" xml:lang="EN-US">(</span>设备创建<span lang="EN-US" xml:lang="EN-US">/</span>销毁<span lang="EN-US" xml:lang="EN-US">,property</span>设置，进程重启<span lang="EN-US" xml:lang="EN-US">)</span>。</p>
  <p><span lang="EN-US" xml:lang="EN-US">Actions</span>表现形式为：</p>
  <p><span lang="EN-US" xml:lang="EN-US">on &lt;trigger&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> &lt;command&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> &lt;command&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> &lt;command&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><strong><span lang="EN-US" xml:lang="EN-US">Services</span></strong></p>
  <p><span lang="EN-US" xml:lang="EN-US">--------</span></p>
  <p><span lang="EN-US" xml:lang="EN-US">Services</span>是由<span lang="EN-US" xml:lang="EN-US">init</span>启动，在它们退出时重启<span lang="EN-US" xml:lang="EN-US">(</span>可选<span lang="EN-US" xml:lang="EN-US">)</span>。<span lang="EN-US" xml:lang="EN-US">Service</span>表现形式为<span lang="EN-US" xml:lang="EN-US">:</span></p>
  <p><span lang="EN-US" xml:lang="EN-US">service &lt;name&gt; &lt;pathname&gt; [ &lt;argument&gt;   ]*</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> &lt;option&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> &lt;option&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> ...</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><strong><span lang="EN-US" xml:lang="EN-US">Options</span></strong></p>
  <p><span lang="EN-US" xml:lang="EN-US">-------</span></p>
  <p><span lang="EN-US" xml:lang="EN-US">Options</span>是<span lang="EN-US" xml:lang="EN-US">Services</span>的修饰，它们影响<span lang="EN-US" xml:lang="EN-US">init</span>何时、如何运行<span lang="EN-US" xml:lang="EN-US">service.</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">critical</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>这是一个设备关键服务<span lang="EN-US" xml:lang="EN-US">(device-critical service) .</span>如果它在<span lang="EN-US" xml:lang="EN-US">4</span>分钟内退出超过<span lang="EN-US" xml:lang="EN-US">4</span>次，设备将重启并进入恢复模式。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">disabled</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>这个服务的级别将不会自动启动，它必须被依照服务名指定启动才可以启动。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">setenv &lt;name&gt; &lt;value&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>设置已启动的进程的环境变量<span lang="EN-US" xml:lang="EN-US">&lt;name&gt;</span>的值<span lang="EN-US" xml:lang="EN-US">&lt;value&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">socket &lt;name&gt; &lt;type&gt; &lt;perm&gt; [ &lt;user&gt;   [ &lt;group&gt; ] ]</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>创建一个名为<span lang="EN-US" xml:lang="EN-US">/dev/socket/&lt;name&gt;</span>的<span lang="EN-US" xml:lang="EN-US">unix   domin socket</span>，并传送它的<span lang="EN-US" xml:lang="EN-US">fd</span>到已启动的进程。<span lang="EN-US" xml:lang="EN-US">&lt;type&gt;</span>必须为<span lang="EN-US" xml:lang="EN-US">&quot;dgram&quot;</span>或<span lang="EN-US" xml:lang="EN-US">&quot;stream&quot;.</span>用户和组默认为<span lang="EN-US" xml:lang="EN-US">0.</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">user &lt;username&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>在执行服务前改变用户名。当前默认为<span lang="EN-US" xml:lang="EN-US">root.</span>如果你的进程需要<span lang="EN-US" xml:lang="EN-US">linux</span>能力，你不能使用这个命令。你必须在还是<span lang="EN-US" xml:lang="EN-US">root</span>时请求能力，并下降到你需要的<span lang="EN-US" xml:lang="EN-US">uid.</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">group &lt;groupname&gt; [ &lt;groupname&gt; ]*</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>在执行服务前改变组。在第一个组后的组将设为进程附加组<span lang="EN-US" xml:lang="EN-US">(</span>通过<span lang="EN-US" xml:lang="EN-US">setgroups()).</span>当前默认为<span lang="EN-US" xml:lang="EN-US">root.</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">oneshot</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>在服务退出后不重启。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">class &lt;name&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>为<span lang="EN-US" xml:lang="EN-US">service</span>指定一个类别名。同样类名的所有的服务可以一起启动或停止。如果没有指定类别的服务默认为<span lang="EN-US" xml:lang="EN-US">&quot;default&quot;</span>类。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">onrestart</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>当服务重启时执行一个命令。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><strong><span lang="EN-US" xml:lang="EN-US">Triggers</span></strong></p>
  <p><span lang="EN-US" xml:lang="EN-US">--------</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> Triggers(</span>触发器<span lang="EN-US" xml:lang="EN-US">)</span>是一个字符串，可以用来匹配某种类型的事件并执行一个<span lang="EN-US" xml:lang="EN-US">action</span>。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">boot</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>这是当<span lang="EN-US" xml:lang="EN-US">init</span>开始后执行的第一个触发器<span lang="EN-US" xml:lang="EN-US">(</span>当<span lang="EN-US" xml:lang="EN-US">/init.conf</span>被加载<span lang="EN-US" xml:lang="EN-US">)</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">&lt;name&gt;=&lt;value&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>当<span lang="EN-US" xml:lang="EN-US">property   &lt;name&gt;</span>被设为指定的值<span lang="EN-US" xml:lang="EN-US">&lt;value&gt;</span>时触发。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">device-added-&lt;path&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US">device-removed-&lt;path&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>当设备节点被添加或移除时触发。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">service-exited-&lt;name&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>当指定的服务存在时触发</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><strong><span lang="EN-US" xml:lang="EN-US">Commands</span></strong></p>
  <p><span lang="EN-US" xml:lang="EN-US">--------</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">exec &lt;path&gt; [ &lt;argument&gt; ]*</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> Fork</span>并执行一个程序<span lang="EN-US" xml:lang="EN-US">(&lt;path&gt;).</span>这将被<span lang="EN-US" xml:lang="EN-US">block</span>直到程序执行完毕。最好避免执行例如内建命令以外的程序，它可能会导致<span lang="EN-US" xml:lang="EN-US">init</span>被阻塞不动。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">export &lt;name&gt; &lt;value&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>设定全局环境变量<span lang="EN-US" xml:lang="EN-US">&lt;name&gt;</span>的值<span lang="EN-US" xml:lang="EN-US">&lt;value&gt;</span>，当这个命令执行后所有的进程都可以取得。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">ifup &lt;interface&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>使网络接口<span lang="EN-US" xml:lang="EN-US">&lt;interface&gt;</span>联机。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">import &lt;filename&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>解析一个<span lang="EN-US" xml:lang="EN-US">init</span>配置文件，扩展当前配置文件。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">hostname &lt;name&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>设置主机名</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">chmod &lt;octal-mode&gt; &lt;path&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>改变文件访问权限</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">chown &lt;owner&gt; &lt;group&gt; &lt;path&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>改变文件所属和组</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">class_start &lt;serviceclass&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>当指定类别的服务没有运行，启动该类别所有的服务。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">class_stop &lt;serviceclass&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>当指定类别的服务正在运行，停止该类别所有的服务。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">domainname &lt;name&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>设置域名。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">insmod &lt;path&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>加载该路径<span lang="EN-US" xml:lang="EN-US">&lt;path&gt;</span>的模块</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">mkdir &lt;path&gt; [mode] [owner] [group]</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>在<span lang="EN-US" xml:lang="EN-US">&lt;path&gt;</span>创建一个目录<span lang="EN-US" xml:lang="EN-US">,</span>可选选项<span lang="EN-US" xml:lang="EN-US">:mod,owner,group.</span>如果没有指定，目录以<span lang="EN-US" xml:lang="EN-US">755</span>权限，<span lang="EN-US" xml:lang="EN-US">owner</span>为<span lang="EN-US" xml:lang="EN-US">root,group</span>为<span lang="EN-US" xml:lang="EN-US">root</span>创建<span lang="EN-US" xml:lang="EN-US">.</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">mount &lt;type&gt; &lt;device&gt; &lt;dir&gt; [   &lt;mountoption&gt; ]*</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>尝试<span lang="EN-US" xml:lang="EN-US">mount   &lt;device&gt;</span>到目录<span lang="EN-US" xml:lang="EN-US">&lt;dir&gt;.   &lt;device&gt;</span>可以用<span lang="EN-US" xml:lang="EN-US"><a href="mailto:mtd@name" target="_blank">mtd@name</a></span>格式以命名指定一个<span lang="EN-US" xml:lang="EN-US">mtd</span>块设备。<span lang="EN-US" xml:lang="EN-US">&lt;mountoption&gt;</span>包含<span lang="EN-US" xml:lang="EN-US">&quot;ro&quot;,&quot;rw&quot;,&quot;remount&quot;,&quot;noatime&quot;.</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">setkey</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>暂时没有</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">setprop &lt;name&gt; &lt;value&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>设置系统<span lang="EN-US" xml:lang="EN-US">property   &lt;name&gt;</span>的值<span lang="EN-US" xml:lang="EN-US">&lt;value&gt;.</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">setrlimit &lt;resource&gt; &lt;cur&gt;   &lt;max&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>设置<span lang="EN-US" xml:lang="EN-US">resource</span>的<span lang="EN-US" xml:lang="EN-US">rlimit.</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">start &lt;service&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>启动一个没有运行的服务。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">stop &lt;service&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>停止一个正在运行的服务。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">symlink &lt;target&gt; &lt;path&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>创建一个<span lang="EN-US" xml:lang="EN-US">&lt;path&gt;</span>的符号链接到<span lang="EN-US" xml:lang="EN-US">&lt;target&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">sysclktz &lt;mins_west_of_gmt&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>设置系统时区<span lang="EN-US" xml:lang="EN-US">(GMT</span>为<span lang="EN-US" xml:lang="EN-US">0)</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">trigger &lt;event&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>触发一个事件。用于调用其它<span lang="EN-US" xml:lang="EN-US">action</span>。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">write &lt;path&gt; &lt;string&gt; [ &lt;string&gt;   ]*</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>打开<span lang="EN-US" xml:lang="EN-US">&lt;path&gt;</span>的文件并写入一个或多个字符串。</p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><strong><span lang="EN-US" xml:lang="EN-US">Properties</span></strong></p>
  <p><span lang="EN-US" xml:lang="EN-US">----------</span></p>
  <p><span lang="EN-US" xml:lang="EN-US">Init</span>会更新一些系统<span lang="EN-US" xml:lang="EN-US">property</span>以提供查看它正在干嘛。</p>
  <p><span lang="EN-US" xml:lang="EN-US">init.action</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>当前正在执行的<span lang="EN-US" xml:lang="EN-US">action,</span>如果没有则为<span lang="EN-US" xml:lang="EN-US">&quot;&quot;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">init.command</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>被执行的命令，如果没有则为<span lang="EN-US" xml:lang="EN-US">&quot;&quot;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">init.svc.&lt;name&gt;</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span>命名为<span lang="EN-US" xml:lang="EN-US">&lt;name&gt;</span>的服务的状态<span lang="EN-US" xml:lang="EN-US">(&quot;stopped&quot;,   &quot;running&quot;, &quot;restarting&quot;)</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><strong><span lang="EN-US" xml:lang="EN-US">init.rc </span></strong><strong>示例</strong><strong><span lang="EN-US" xml:lang="EN-US">:</span></strong></p>
  <p><span lang="EN-US" xml:lang="EN-US">-----------------</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US"># not complete -- just providing some examples of   usage</span></p>
  <p><span lang="EN-US" xml:lang="EN-US">#</span></p>
  <p><span lang="EN-US" xml:lang="EN-US">on boot</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> export PATH /sbin:/system/sbin:/system/bin</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> export LD_LIBRARY_PATH /system/lib</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> mkdir /dev</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> mkdir /proc</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> mkdir /sys</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> mount tmpfs tmpfs /dev</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> mkdir /dev/pts</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> mkdir /dev/socket</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> mount devpts devpts /dev/pts</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> mount proc proc /proc</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> mount sysfs sysfs /sys</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> write /proc/cpu/alignment 4</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> ifup lo</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> hostname localhost</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> domainname localhost</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> mount yaffs2 <a href="mailto:mtd@system" target="_blank">mtd@system</a> /system</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> mount yaffs2 <a href="mailto:mtd@userdata" target="_blank">mtd@userdata</a> /data</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> import /system/etc/init.conf</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> class_start default</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">service adbd /sbin/adbd</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> user adb</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> group adb</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">service usbd /system/bin/usbd -r</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> user usbd</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> group usbd</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> socket usbd 666</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">service zygote /system/bin/app_process -Xzygote /system/bin   --zygote</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> socket zygote 666</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">service runtime /system/bin/runtime</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> user system</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> group system</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">on device-added-/dev/compass</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> start akmd</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">on device-removed-/dev/compass</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> stop akmd</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><span lang="EN-US" xml:lang="EN-US">service akmd /sbin/akmd</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> disabled</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> user akmd</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> group akmd</span></p>
  <p><span lang="EN-US" xml:lang="EN-US"> </span></p>
  <p><strong>调试</strong></p>
  <p><span lang="EN-US" xml:lang="EN-US">---------------</span></p>
  <p>默认情况下，<span lang="EN-US" xml:lang="EN-US">init</span>执行的程序输出的信息和错误到<span lang="EN-US" xml:lang="EN-US">/dev/null.</span>为了<span lang="EN-US" xml:lang="EN-US">debug</span>，你可以通过<span lang="EN-US" xml:lang="EN-US">Android</span>程序<span lang="EN-US" xml:lang="EN-US">logwrapper</span>执行你的程序。这将复位向输出<span lang="EN-US" xml:lang="EN-US">/</span>错误输出到<span lang="EN-US" xml:lang="EN-US">Android   logging</span>系统<span lang="EN-US" xml:lang="EN-US">(</span>通过<span lang="EN-US" xml:lang="EN-US">logcat</span>访问<span lang="EN-US" xml:lang="EN-US">)</span>。</p>
  <p>例如</p>
  <p><span lang="EN-US" xml:lang="EN-US">service akmd /system/bin/logwrapper /sbin/akmd</span></p>
</div>
<h1>&nbsp;</h1>
  <!-- end #mainContent --></div>
  <div id="footer">
<p>脚注</p>
  <!-- end #footer --></div>
<!-- end #container --></div>
</body>
</html>
