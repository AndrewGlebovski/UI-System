/**
 * \file
 * \brief Contains base action class and prototypes of his functions
*/


/// Base class for all actions
class BaseAction {
public:
    /**
     * \brief Does something
    */
    virtual void operator () () = 0;


    /**
     * \brief Destroyes action
    */
    virtual ~BaseAction() = default;
};
