#ifndef LM_TLIST_HPP
#define LM_TLIST_HPP

/**===========================================================================
 * Tiny singly-linked list used for passing around lists with only a pointer
 * copy.
 *
 * @author Logan Moore
 * @version 1.0
 ============================================================================*/


template <class T> class TList {
public:
    TList(const T& p_val){ _value = p_val; _next = nullptr; }

    /** @returns [T] The value of this list instance. */
    T value() { return _value; }
    /** @returns [T* ] Address of the list instance's value. */
    T* valptr() { return &_value; }

    /**
    * Get a pointer to the next instance in the list.
    *
    * @returns [Tlist<T>* ] Pointer to the next instance in the list.
    */
    TList<T>* next() { return _next; }


    /**
    * Iterate over the rest of the list and return the last instance linked.
    *
    * @returns [TList<T>* ] Pointer to the last instance in the list.
    */
    TList<T>* last() {
        if(_next == nullptr)
            return this;

        TList<T>* temp = this;
        while (temp->next() != nullptr)
            temp = temp->next();
        return temp;
    }

    /**
    * Creates a new list instance of the value passed to the function, links
    * it as the next list instance and returns its address.
    *
    * !WARNING! Appending to an list instance that isn't the last instance in
    * the list leaks the rest of the list!
    *
    * @param p_val The value the new instance will hold.
    * @returns [Tlist<T>* ] Pointer to created list instance.
    */
    TList<T>* append(const T& p_val){ return _next = new TList<T>(p_val); }
private:
    T _value; //@- Value of this list instance.
    TList<T>* _next; //@- Link to next list instance.
};

#endif
