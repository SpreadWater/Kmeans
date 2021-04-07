#include <stdio.h>

#define MaxTeamNumber 15//球队数量
#define MaxYearNumber 3//年份数量
#define MaxCenterClusterNumber 3//选取的中心簇数量
#define MaxNumber 1000.0//定义的最大成绩值
#define MinNumber 0.0//定义的最小成绩值

typedef double GRADE_TYPE;//重定义成绩的数据类型
typedef double INSTANCE_TYPE;//球队到中心簇的距离数据类型
typedef int TEAMID_TYPE;//球队的ID数据类型
typedef int CENTERCLUSTERID_TYPE;//中心簇的ID数据类型
/*
 * 年份结构体
 */
typedef struct year {
    GRADE_TYPE grade;
} Year;

/*
 * 国家结构体
 */
typedef struct team {
    TEAMID_TYPE teamId;//球队的ID
    CENTERCLUSTERID_TYPE centerClusterId;//所属中心簇的ID
    Year years[MaxYearNumber];//年份数组
    INSTANCE_TYPE instances[MaxCenterClusterNumber];//球队到中心簇的距离数组
} Team;

/*
 * 需要用到的一些工具方法
 */
GRADE_TYPE findMax(GRADE_TYPE *grades);//找最大成绩
GRADE_TYPE findMin(GRADE_TYPE *grades);//找最小成绩
/*
 * 传入球队数组，进行规格化处理
 */
void standardGrade(Team *teams);

/*
 * 传入最大值，最小值和待规格化的值
 */
GRADE_TYPE standardData(GRADE_TYPE Max,GRADE_TYPE Min,GRADE_TYPE unStandardData);

/*
 * 初始化中心簇，根据传进来的
 */


int main() {
    printf("Hello, World!\n");
    return 0;
}

GRADE_TYPE findMax(GRADE_TYPE *grades) {
    GRADE_TYPE result = MinNumber;
    for (int i = 0; i < MaxTeamNumber; ++i) {
        if (grades[i] > result) {
            result = grades[i];
        }
    }
    return result;
}

GRADE_TYPE findMin(GRADE_TYPE *grades) {
    GRADE_TYPE result = MaxNumber;
    for (int i = 0; i < MaxTeamNumber; ++i) {
        if (grades[i] < result) {
            result = grades[i];
        }
    }
    return result;
}
GRADE_TYPE standardData(GRADE_TYPE Max,GRADE_TYPE Min,GRADE_TYPE unStandardData){
    GRADE_TYPE result;
    result=(unStandardData-Min)/(Max-Min);
    return result;
}
void standardGrade(Team *teams){
    GRADE_TYPE grades[MaxYearNumber][MaxTeamNumber];//纵向的年份成绩二维数组

    for (int i = 0; i <MaxYearNumber ; ++i) {
        for (int j = 0; j <MaxTeamNumber ; ++j) {
            grades[i][j]=teams[j].years[i].grade;
        }
    }
    //规格化操作
    for (int j = 0; j <MaxTeamNumber ; ++j) {
        for (int i = 0; i < MaxYearNumber; ++i) {
            GRADE_TYPE MaxGrade = findMax(grades[i]);
            GRADE_TYPE MinGrad = findMin(grades[i]);
            //归一化更新操作
            teams[j].years[i].grade=standardData(MaxGrade, MinGrad, teams[j].years[i].grade);
        }
    }
}
