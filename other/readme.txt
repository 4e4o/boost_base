чтоб в qtcreatorе clang code model распознавал корутины c++20 надо запустить креатор с переменной окружения:
export QTC_CLANG_CCM_CMD_APPEND="-include PATH_TO/boost_base/other/_fix_cor_.h"
решение взято частично отсюда:
https://bugreports.qt.io/browse/QTCREATORBUG-24634?gerritReviewStatus=Open
этот баг из-за того что клэнг еще не поддерживает корутины, пока только эскпериментально.
