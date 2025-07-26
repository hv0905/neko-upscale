编写一个realesrgan-ncnn-vulkan的GUI App，要求如下：
1. 用户会通过启动参数传入一个图片文件或图片目录进行处理（预计会绑定到dolphin和nautilus的context menu中），如果用户没有传入，则弹出一个文件选择窗口允许用户选择文件或目录，完成后同样进行下一步操作。
2. 使用qt展示一个简化ui，允许用户调节upscale ratio和output format，然后有一个高级面板可以点击展开，展开后可以额外调节output path和thread count。
3. 默认的输出文件名是 原文件名_倍率x 目录名相同，例如 images_4x 和 foo_4x.png. 如果用户输入的是目录，在目录添加了后缀就无须在文件添加后缀
4. 用户点击开始后，在下方展示一个进度条指示目前超分的进度。如果用户是单文件，展示这个文件的处理进度，如果用户是多文件，进度则按照（完成文件数/总文件数）进行计算，无须精确到每个文件的处理进度。

以下是realesrgan-ncnn-vulkan的帮助：
Usage: realesrgan-ncnn-vulkan -i infile -o outfile [options]...

  -h                   show this help
  -i input-path        input image path (jpg/png/webp) or directory
  -o output-path       output image path (jpg/png/webp) or directory
  -s scale             upscale ratio (can be 2, 3, 4. default=4)
  -t tile-size         tile size (>=32/0=auto, default=0) can be 0,0,0 for multi-gpu
  -m model-path        folder path to the pre-trained models. default=models
  -n model-name        model name (default=realesr-animevideov3, can be realesr-animevideov3 | realesrgan-x4plus | realesrgan-x4plus-anime | realesrnet-x4plus)
  -g gpu-id            gpu device to use (default=auto) can be 0,1,2 for multi-gpu
  -j load:proc:save    thread count for load/proc/save (default=1:2:2) can be 1:2,2,2:2 for multi-gpu
  -x                   enable tta mode
  -f format            output image format (jpg/png/webp, default=ext/png)
  -v                   verbose output

以下是处理一张图片时的样例输出：
```
$ realesrgan-ncnn-vulkan -v -i 84046594_p0.jpg -o output.png
[0 NVIDIA GeForce RTX 3060 Laptop GPU]  queueC=2[8]  queueG=0[16]  queueT=1[2]
[0 NVIDIA GeForce RTX 3060 Laptop GPU]  bugsbn1=0  bugbilz=0  bugcopc=0  bugihfa=0
[0 NVIDIA GeForce RTX 3060 Laptop GPU]  fp16-p/s/a=1/1/1  int8-p/s/a=1/1/1
[0 NVIDIA GeForce RTX 3060 Laptop GPU]  subgroup=32  basic=1  vote=1  ballot=1  shuffle=1
0.00%
0.85%
1.71%
2.56%
3.42%
4.27%
5.13%
(省略大量百分比进度信息)
94.02%
94.87%
95.73%
96.58%
97.44%
98.29%
99.15%
84046594_p0.jpg -> output.png done
```


以下是处理目录时的样例输出：

```
$ realesrgan-ncnn-vulkan -v -i . -o output/
[0 NVIDIA GeForce RTX 3060 Laptop GPU]  queueC=2[8]  queueG=0[16]  queueT=1[2]
[0 NVIDIA GeForce RTX 3060 Laptop GPU]  bugsbn1=0  bugbilz=0  bugcopc=0  bugihfa=0
[0 NVIDIA GeForce RTX 3060 Laptop GPU]  fp16-p/s/a=1/1/1  int8-p/s/a=1/1/1
[0 NVIDIA GeForce RTX 3060 Laptop GPU]  subgroup=32  basic=1  vote=1  ballot=1  shuffle=1
0.00%
0.85%
0.00%
1.71%
（此处省略大量百分比进度信息，值得注意的是这个百分比信息是单个文件的进度，且没有标注是哪个文件，因此没有参考意义。）
99.68%
./98259515_p0.jpg -> output//98259515_p0.png done
./84046594_p0.jpg -> output//84046594_p0.png done
./https___sakuratrak.aiursoft.cn_api_static_db66bd71-7e66-5f41-a7c4-ce8a42edc0ee.jpg -> output//https___sakuratrak.aiursoft.cn_api_static_db66bd71-7e66-5f41-a7c4-ce8a42edc0ee.png done
```

注意：以上所有输出均是打印到stderr中，而不是stdout中，在读取时应读取stderr或直接 2>1

请使用QT6和C++完成编写，初始项目模板已经为你建立好。同时，由于希望应用跨平台（Win, MacOS, Linux），请尽可能不要依赖平台特定接口。