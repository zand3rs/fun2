#!/bin/ksh
stty -isig

#----------------------------------------------------------------------------
# Configurable Variables
#----------------------------------------------------------------------------
main_menu=/appl/fun2/scripts/OPSMainMenuAll4.mnu
current_menu=$main_menu
prompt_to_continue="Press Enter to Continue..."
screen_width=125
title_location_x=0
title_location_y=1
seperator="->"
seperator_length=`echo "$seperator" | awk '{print length($1)}'`
invalid_input_prompt="<<< Invalid Input! >>>"
invalid_input_prompt_y=2
show_text_y=0

display_menu()
{
 clear
 para_name=$1

 #locations of header, items, and input
 let header_loc_x=`awk -F"=" '/header_location_xy/ {print $2}' $para_name | awk -F"," '{print $1}'`
 let header_loc_y=`awk -F"=" '/header_location_xy/ {print $2}' $para_name | awk -F"," '{print $2}'`
 let item_loc_x=`awk -F"=" '/items_location_xy/ {print $2}' $para_name | awk -F"," '{print $1}'`
 let item_loc_y=`awk -F"=" '/items_location_xy/ {print $2}' $para_name | awk -F"," '{print $2}'`
 let line_spaces=`awk -F"=" '/line_spaces/ {print $2}' $para_name`
 let input_prompt_loc_x=`awk -F"=" '/input_prompt_location_xy/ {print $2}' $para_name | awk -F"," '{print $1}'`
 let input_prompt_loc_y=`awk -F"=" '/input_prompt_location_xy/ {print $2}' $para_name | awk -F"," '{print $2}'`
 let input_loc_x=`awk -F"=" '/input_location_xy/ {print $2}' $para_name | awk -F"," '{print $1}'`
 let input_loc_y=`awk -F"=" '/input_location_xy/ {print $2}' $para_name | awk -F"," '{print $2}'`

 #Brackets for items
 left_bracket=`awk -F"=" '/enclosed_brackets/ {print $2}' $para_name | awk -F"," '{print $1}'`
 right_bracket=`awk -F"=" '/enclosed_brackets/ {print $2}' $para_name | awk -F"," '{print $2}'`

 #Display Title
 display_menu_title

 #Display header
 awk '/^<header>/,/^<\/header>/' $para_name | awk '! /<header>/ && ! /<\/header>/' | awk -v header_loc_x=$header_loc_x -v header_loc_y=$header_loc_y '{system("tput cup " header_loc_y " " header_loc_x); print; header_loc_y+=1}'

 #Display menu items
 awk '/^<items_list>/,/^<\/items_list>/' $para_name | awk '! /<items_list>/ && ! /<\/items_list>/' | while read line
 do
  item_id=`echo $line | awk -F"~" '{print $1}'`
  label=`echo $line | awk -F"~" '{print $3}'`

  #Display menu choice
  tput cup $item_loc_y $item_loc_x; echo "$left_bracket""$item_id""$right_bracket" "$label"
  let item_loc_y=$item_loc_y+$line_spaces
 done

 #Input prompt details
 input_prompt=`awk -F"=" '/input_prompt=/ {print $2}' $para_name`
 tput cup $input_prompt_loc_y $input_prompt_loc_x;echo $input_prompt
 tput cup $input_loc_y $input_loc_x;read choice
}

run_command()
{
 clear
 `echo $choice_item_line | awk -F "~" '{print $4}'`
 let no_of_fields=`echo $choice_item_line | awk -F"~" '{print NF}'`
 if [ $no_of_fields -eq 5 ];then
  let location_prompt_to_continue_x=`echo $choice_item_line | awk -F"~" '{print $NF}' | awk -F"," '{print $1}'`
  let location_prompt_to_continue_y=`echo $choice_item_line | awk -F"~" '{print $NF}' | awk -F"," '{print $2}'`
  let location_prompt_to_continue_read_x=`echo $prompt_to_continue | awk '{print length($0)}'`+$location_prompt_to_continue_x
  tput cup $location_prompt_to_continue_y $location_prompt_to_continue_x; echo $prompt_to_continue
  tput cup $location_prompt_to_continue_y $location_prompt_to_continue_read_x; read choice_2
 fi
 run_menu $current_menu
}

validate_choice()
{
 choice_item_line=`awk '/^<items_list>/,/^<\/items_list>/' $para_name | awk -F "~" -v choice=$choice '! /<items_list>/ && ! /<\/items_list>/ {if($1==choice) print $0}'`
 choice_item_id=`echo $choice_item_line | awk -F "~" '{print $1}'`
 choice_item_type=`echo $choice_item_line | awk -F "~" '{print $2}'`
 choice_item_label=`echo $choice_item_line | awk -F "~" '{print $3}'`
 choice_item_cmdmnu=`echo $choice_item_line | awk -F "~" '{print $4}'`
 case $choice_item_type in
  "c") run_command;;
  "m") run_menu $choice_item_cmdmnu;;
 esac
 if [ -z "$choice_item_line" ];then
  show_text_y=2
  show_text $invalid_input_prompt
  sleep 1
  run_menu $current_menu
 fi
}

run_menu()
{
 clear
 current_menu=$1
 display_menu $1
 validate_choice
}

show_text()
{
 current_title="$*"
 let title_length=`echo $current_title | awk '{print length($0)}'`
 let title_location_x=`echo $screen_width $title_length | awk '{printf "%d\n", ($1-$2)/2}'`
 #tput cup $show_text_y $title_location_x
 tput cup $show_text_y 5 $title_location_x 35
 echo $current_title
}

display_menu_title()
{
 menu_loop=$current_menu
 title_to_display=""
 while [ ! -z "$menu_loop" ]
 do
  menu_title=`awk -F= '/^menu_title/ {print $2}' $menu_loop`
  parent_menu=`awk -F= '/^parent_menu/ {print $2}' $menu_loop`
  title_to_display="$menu_title""$seperator""$title_to_display"
  menu_loop=$parent_menu
 done
 let title_to_display_length=`echo "$title_to_display" | awk '{print length($0)}'`
 title_to_display=`echo "$title_to_display" | awk -v l=$title_to_display_length -v ls=$seperator_length '{print substr($0,1,l-ls)}'`
 let show_text_y=$title_location_y
 show_text $title_to_display
}

run_menu $main_menu

