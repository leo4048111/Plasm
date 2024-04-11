// SPDX-License-Identifier: MIT
// OpenZeppelin Contracts (last updated v5.0.0) (token/ERC20/ERC20.sol)

pragma solidity >0.4.23;

/**
 * @dev Implementation of the {IERC20} interface.
 *
 * This implementation is agnostic to the way tokens are created. This means
 * that a supply mechanism has to be added in a derived contract using {_mint}.
 *
 * TIP: For a detailed writeup see our guide
 * https://forum.openzeppelin.com/t/how-to-implement-erc20-supply-mechanisms/226[How
 * to implement supply mechanisms].
 *
 * The default value of {decimals} is 18. To change this, you should override
 * this function so it returns a different value.
 *
 * We have followed general OpenZeppelin Contracts guidelines: functions revert
 * instead returning `false` on failure. This behavior is nonetheless
 * conventional and does not conflict with the expectations of ERC-20
 * applications.
 */
abstract contract ERC20{
    uint256 private _balancesFrom;
    uint256 private _balancesTo;

    uint256 private _allowancesSpender;
    uint256 private _allowancesReceiver;

    uint256 private _totalSupply;

    string private _name;
    string private _symbol;

    error ERC20InvalidSender(address sender);
    error ERC20InvalidApprover(address approver);
    error ERC20InvalidSpender(address spender);
    error ERC20InvalidReceiver(address receiver);
    error ERC20InsufficientBalance(address addr, uint256 val1, uint256 val2);
    error ERC20InsufficientAllowance(address spender, uint256 currentAllownace, uint256 value);

    // /**
    //  * @dev Moves a `value` amount of tokens from `from` to `to`.
    //  *
    //  * This internal function is equivalent to {transfer}, and can be used to
    //  * e.g. implement automatic token fees, slashing mechanisms, etc.
    //  *
    //  * Emits a {Transfer} event.
    //  *
    //  * NOTE: This function is not virtual, {_update} should be overridden instead.
    //  */
    function _transfer(address from, address to, uint256 value) internal {
        if (from == address(0)) {
            revert ERC20InvalidSender(address(0));
        }
        if (to == address(0)) {
            revert ERC20InvalidReceiver(address(0));
        }
        // _update(from, to, value);
    }

    /**
     * @dev See {IERC20-transfer}.
     *
     * Requirements:
     *
     * - `to` cannot be the zero address.
     * - the caller must have a balance of at least `value`.
     */
    function transfer(address to, uint256 value) public virtual returns (bool) {
        address owner = msg.sender;
        _transfer(owner, to, value);
        return true;
    }

    // /**
    //  * @dev See {IERC20-approve}.
    //  *
    //  * NOTE: If `value` is the maximum `uint256`, the allowance is not updated on
    //  * `transferFrom`. This is semantically equivalent to an infinite approval.
    //  *
    //  * Requirements:
    //  *
    //  * - `spender` cannot be the zero address.
    //  */
    function approve(address spender, uint256 value) public virtual returns (bool) {
        address owner = msg.sender;
        _approve(owner, spender, value);
        return true;
    }

    // /**
    //  * @dev See {IERC20-allowance}.
    //  */
    // function allowance(address owner, address spender) public view virtual returns (uint256) {
    //     return _allowancesSpender;
    // }

    // /**
    //  * @dev See {IERC20-transferFrom}.
    //  *
    //  * Skips emitting an {Approval} event indicating an allowance update. This is not
    //  * required by the ERC. See {xref-ERC20-_approve-address-address-uint256-bool-}[_approve].
    //  *
    //  * NOTE: Does not update the allowance if the current allowance
    //  * is the maximum `uint256`.
    //  *
    //  * Requirements:
    //  *
    //  * - `from` and `to` cannot be the zero address.
    //  * - `from` must have a balance of at least `value`.
    //  * - the caller must have allowance for ``from``'s tokens of at least
    //  * `value`.
    //  */
    // function transferFrom(address from, address to, uint256 value) public virtual returns (bool) {
    //     address spender = msg.sender;
    //     _spendAllowance(from, spender, value);
    //     _transfer(from, to, value);
    //     return true;
    // }

    // /**
    //  * @dev Transfers a `value` amount of tokens from `from` to `to`, or alternatively mints (or burns) if `from`
    //  * (or `to`) is the zero address. All customizations to transfers, mints, and burns should be done by overriding
    //  * this function.
    //  *
    //  * Emits a {Transfer} event.
    //  */
    // function _update(address from, address to, uint256 value) internal virtual {
    //     if (from == address(0)) {
    //         // Overflow check required: The rest of the code assumes that totalSupply never overflows
    //         _totalSupply += value;
    //     } else {
    //         uint256 fromBalance = _balancesFrom;
    //         if (fromBalance < value) {
    //             revert ERC20InsufficientBalance(from, fromBalance, value);
    //         }

    //         // Overflow not possible: value <= fromBalance <= totalSupply.
    //         _balancesFrom = fromBalance - value;
    //     }

    //     if (to == address(0)) {
    //         unchecked {
    //             // Overflow not possible: value <= totalSupply or value <= fromBalance <= totalSupply.
    //             _totalSupply -= value;
    //         }
    //     } else {
    //         unchecked {
    //             // Overflow not possible: balance + value is at most totalSupply, which we know fits into a uint256.
    //             _balancesFrom += value;
    //         }
    //     }
    // }

    // /**
    //  * @dev Creates a `value` amount of tokens and assigns them to `account`, by transferring it from address(0).
    //  * Relies on the `_update` mechanism
    //  *
    //  * Emits a {Transfer} event with `from` set to the zero address.
    //  *
    //  * NOTE: This function is not virtual, {_update} should be overridden instead.
    //  */
    // function _mint(address account, uint256 value) internal {
    //     if (account == address(0)) {
    //         revert ERC20InvalidReceiver(address(0));
    //     }
    //     _update(address(0), account, value);
    // }

    // /**
    //  * @dev Destroys a `value` amount of tokens from `account`, lowering the total supply.
    //  * Relies on the `_update` mechanism.
    //  *
    //  * Emits a {Transfer} event with `to` set to the zero address.
    //  *
    //  * NOTE: This function is not virtual, {_update} should be overridden instead
    //  */
    // function _burn(address account, uint256 value) internal {
    //     if (account == address(0)) {
    //         revert ERC20InvalidSender(address(0));
    //     }
    //     _update(account, address(0), value);
    // }

    // /**
    //  * @dev Sets `value` as the allowance of `spender` over the `owner` s tokens.
    //  *
    //  * This internal function is equivalent to `approve`, and can be used to
    //  * e.g. set automatic allowances for certain subsystems, etc.
    //  *
    //  * Emits an {Approval} event.
    //  *
    //  * Requirements:
    //  *
    //  * - `owner` cannot be the zero address.
    //  * - `spender` cannot be the zero address.
    //  *
    //  * Overrides to this logic should be done to the variant with an additional `bool emitEvent` argument.
    //  */
    // function _approve(address owner, address spender, uint256 value) internal {
    //     _approve(owner, spender, value, true);
    // }

    // /**
    //  * @dev Variant of {_approve} with an optional flag to enable or disable the {Approval} event.
    //  *
    //  * By default (when calling {_approve}) the flag is set to true. On the other hand, approval changes made by
    //  * `_spendAllowance` during the `transferFrom` operation set the flag to false. This saves gas by not emitting any
    //  * `Approval` event during `transferFrom` operations.
    //  *
    //  * Anyone who wishes to continue emitting `Approval` events on the`transferFrom` operation can force the flag to
    //  * true using the following override:
    //  *
    //  * ```solidity
    //  * function _approve(address owner, address spender, uint256 value, bool) internal virtual override {
    //  *     super._approve(owner, spender, value, true);
    //  * }
    //  * ```
    //  *
    //  * Requirements are the same as {_approve}.
    //  */
    function _approve(address owner, address spender, uint256 value) internal virtual {
        if (owner == address(0)) {
            revert ERC20InvalidApprover(address(0));
        }
        if (spender == address(0)) {
            revert ERC20InvalidSpender(address(0));
        }
        _allowancesReceiver = value;
    }

    // /**
    //  * @dev Updates `owner` s allowance for `spender` based on spent `value`.
    //  *
    //  * Does not update the allowance value in case of infinite allowance.
    //  * Revert if not enough allowance is available.
    //  *
    //  * Does not emit an {Approval} event.
    //  */
    // function _spendAllowance(address owner, address spender, uint256 value) internal virtual {
    //     uint256 currentAllowance = allowance(owner, spender);
    //     if (currentAllowance != type(uint256).max) {
    //         if (currentAllowance < value) {
    //             revert ERC20InsufficientAllowance(spender, currentAllowance, value);
    //         }
    //         unchecked {
    //             _approve(owner, spender, currentAllowance - value, false);
    //         }
    //     }
    // }
}