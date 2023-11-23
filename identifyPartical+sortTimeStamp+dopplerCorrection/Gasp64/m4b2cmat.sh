#!/bin/bash

m4b_path="/home/share_data/40Ca106Cd_guos/data2/m4b/PAM/"
cmat_path="../../cmat/PAM/" # 字符串不支持过长

# 使用 find 命令获取所有 .m4b 文件的列表
m4b_files=$(find "$m4b_path" -type f -name "*.m4b")

# delete existed .cmat
rm -rf "$cmat_path"*

# 遍历文件列表
for file in $m4b_files; do
    # 检查文件是否存在
    if [ -e "$file" ]; then
        # 提取文件名
        filename=$(basename -- "$file")
		# echo $filename

        # 构建输出文件路径
        output_file="${cmat_path}${filename%.m4b}.cmat" 

        # # 如果输出文件存在，则删除
        # if [ -e "$output_file" ]; then
        #     echo "Deleting existing file: $output_file"
        #     rm -f "$output_file"
        # fi

        # 打印提示信息
        echo "Processing file: $filename"

        # 运行 cmat 程序并传递输入
        cmat <<EOF
Compress_2d $filename
4096|4096
4
Y
256
$output_file
EOF

        # 等待 cmat 程序完成
        # wait $!
        wait
		# sleep 1
        # 打印分隔行
    #     echo "---------------------"
    # else
    #     echo "File not found: $file"
    fi
done
